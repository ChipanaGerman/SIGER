/**
 * @file   SimuladorEntregas.cpp
 * @brief  Implementación de SimuladorEntregas — motor multihilo de SIGER.
 *
 * Cada repartidor corre en su propio std::thread ejecutando hiloRepartidor().
 * La sincronización productor-consumidor se realiza con m_mutex +
 * m_condVar; la pausa/reanudación usa m_pausado (atomic<bool>).
 *
 * @author SIGER Dev Team
 */

#include "SimuladorEntregas.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <string>

namespace siger {

// ── Constructor / Destructor ──────────────────────────────────────────────────

SimuladorEntregas::SimuladorEntregas(
    std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola)
    : m_colaPedidos(std::move(cola))
{
    if (!m_colaPedidos) {
        throw std::invalid_argument(
            "SimuladorEntregas: la cola de pedidos no puede ser nula.");
    }
}

SimuladorEntregas::~SimuladorEntregas()
{
    // Garantiza que los hilos se detengan antes de destruir el objeto.
    detener();
}

// ── Gestión de repartidores ───────────────────────────────────────────────────

void SimuladorEntregas::agregarRepartidor(std::shared_ptr<Repartidor> repartidor)
{
    if (!repartidor) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_corriendo.load()) {
        // La simulación ya arrancó: lanzar hilo inmediatamente.
        m_repartidores.push_back(repartidor);
        m_hilos.emplace_back(&SimuladorEntregas::hiloRepartidor, this, repartidor);
    } else {
        m_repartidores.push_back(repartidor);
    }
}

// ── Control de la simulación ──────────────────────────────────────────────────

void SimuladorEntregas::iniciar()
{
    // Idempotente: evita doble arranque.
    if (m_corriendo.exchange(true)) return;

    m_pausado.store(false);
    m_totalEntregados.store(0);
    m_totalFallidos.store(0);
    m_enCamino.store(0);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& rep : m_repartidores) {
            m_hilos.emplace_back(&SimuladorEntregas::hiloRepartidor, this, rep);
        }
    }

    notificarObservers("SIMULACION_INICIADA");
}

void SimuladorEntregas::pausar()
{
    if (!m_corriendo.load()) return;

    m_pausado.store(true);
    notificarObservers("SIMULACION_PAUSADA");
    // No notificamos m_condVar: los hilos que ya están en sleep seguirán
    // hasta completar su viaje actual; los que esperan entrada se bloquearán.
}

void SimuladorEntregas::reanudar()
{
    if (!m_corriendo.load()) return;

    m_pausado.store(false);
    m_condVar.notify_all();   // desbloquea hilos esperando en condVar
    notificarObservers("SIMULACION_REANUDADA");
}

void SimuladorEntregas::detener()
{
    if (!m_corriendo.exchange(false)) return;  // ya estaba detenido

    m_pausado.store(false);
    m_condVar.notify_all();   // despierta todos los hilos para que salgan

    // Join ordenado de todos los hilos.
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& hilo : m_hilos) {
        if (hilo.joinable()) {
            hilo.join();
        }
    }
    m_hilos.clear();

    notificarObservers("SIMULACION_DETENIDA");
}

// ── Configuración ─────────────────────────────────────────────────────────────

void SimuladorEntregas::setVelocidad(int ms)
{
    // Mínimo 100 ms para evitar espins.
    m_velocidadSimulacion = std::max(100, ms);
}

// ── Consultas ─────────────────────────────────────────────────────────────────

bool SimuladorEntregas::isCorriendo() const
{
    return m_corriendo.load() && !m_pausado.load();
}

EstadisticasSimulacion SimuladorEntregas::getEstadisticas() const
{
    return EstadisticasSimulacion {
        m_totalEntregados.load(),
        m_totalFallidos.load(),
        m_enCamino.load()
    };
}

// ── Función de hilo ───────────────────────────────────────────────────────────

void SimuladorEntregas::hiloRepartidor(std::shared_ptr<Repartidor> rep)
{
    // Lambda que simula el trayecto de entrega durmiendo el tiempo proporcional
    // a la distancia.  Captura por referencia los miembros del simulador.
    auto simularViaje = [&](double distanciaKm) {
        const int duracionMs =
            static_cast<int>(distanciaKm * m_velocidadSimulacion);
        std::this_thread::sleep_for(std::chrono::milliseconds(duracionMs));
    };

    while (m_corriendo.load()) {
        std::shared_ptr<Pedido> pedido;

        // ── Esperar un pedido disponible ──────────────────────────────────────
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // Predicado: salir de la espera cuando haya trabajo real o se deba
            // terminar.  También respeta la bandera de pausa.
            m_condVar.wait(lock, [this] {
                return !m_corriendo.load()                     // petición de stop
                    || (!m_pausado.load()                      // no pausado
                        && !m_colaPedidos->isEmpty());         // hay pedidos
            });

            // Si nos despertaron para detener, salir del loop.
            if (!m_corriendo.load()) break;

            // Extraer el siguiente pedido de la cola.
            auto optPedido = m_colaPedidos->pop();
            if (!optPedido.has_value()) {
                // Falsa alarma (race con otro hilo): volver a esperar.
                continue;
            }
            pedido = std::move(optPedido.value());
        }

        if (!pedido) continue;

        try {
            // ── 1. Marcar EN_CAMINO ───────────────────────────────────────────
            pedido->setEstado(EstadoPedido::EN_CAMINO);
            pedido->setRepartidorId(rep->getId());
            ++m_enCamino;

            notificarObservers(
                "EN_CAMINO:" + pedido->getId() + ":" + rep->getId());

            // ── 2. Simular el viaje ───────────────────────────────────────────
            const double distancia = pedido->getDistanciaKm();
            simularViaje(distancia);

            // ── 3. Comprobar si se detuvo durante el viaje ────────────────────
            if (!m_corriendo.load()) {
                // Marcar fallido al abortar.
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                notificarObservers(
                    "PEDIDO_FALLIDO:" + pedido->getId() + ":" + rep->getId());
                break;
            }

            // ── 4. Marcar COMPLETADO ──────────────────────────────────────────
            pedido->setEstado(EstadoPedido::COMPLETADO);
            --m_enCamino;
            ++m_totalEntregados;

            notificarObservers(
                "PEDIDO_COMPLETADO:" + pedido->getId() + ":" + rep->getId());

        } catch (const std::exception& ex) {
            // Error inesperado: registrar como fallido y continuar.
            if (pedido) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                notificarObservers(
                    "PEDIDO_FALLIDO:" + pedido->getId() + ":" + rep->getId()
                    + " [ex:" + ex.what() + "]");
            }
        } catch (...) {
            if (pedido) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                notificarObservers(
                    "PEDIDO_FALLIDO:" + pedido->getId() + ":" + rep->getId()
                    + " [error desconocido]");
            }
        }

        // Notificar a otros hilos que puede haber cambios en la cola.
        m_condVar.notify_all();
    }
}

} // namespace siger
