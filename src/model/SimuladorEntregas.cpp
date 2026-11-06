/**
 * @file SimuladorEntregas.cpp
 * @brief Implementación de SimuladorEntregas — motor multihilo de SIGER.
 */

#include "SimuladorEntregas.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <string>

namespace siger {

SimuladorEntregas::SimuladorEntregas(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola)
    : m_colaPedidos(std::move(cola))
{
    if (!m_colaPedidos) {
        throw std::invalid_argument("SimuladorEntregas: la cola de pedidos no puede ser nula.");
    }
}

SimuladorEntregas::~SimuladorEntregas() {
    detener();
}

void SimuladorEntregas::onEvento(const DatoEvento& evento) {
    if (evento.tipo == EventoSiger::PEDIDO_NUEVO) {
        despertar();
    }
}

void SimuladorEntregas::despertar() {
    m_condVar.notify_all();
}

void SimuladorEntregas::agregarRepartidor(std::shared_ptr<Repartidor> repartidor) {
    if (!repartidor) return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_repartidores.push_back(repartidor);

    if (m_corriendo.load()) {
        m_hilos.emplace_back(&SimuladorEntregas::hiloRepartidor, this, repartidor);
    }
}

void SimuladorEntregas::iniciar() {
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

    notificarObservers(EventoSiger::SIMULACION_INICIADA, "", "Simulación iniciada");
}

void SimuladorEntregas::pausar() {
    if (!m_corriendo.load()) return;

    m_pausado.store(true);
    notificarObservers(EventoSiger::SIMULACION_PAUSADA, "", "Simulación pausada");
}

void SimuladorEntregas::reanudar() {
    if (!m_corriendo.load()) return;

    m_pausado.store(false);
    m_condVar.notify_all();
    notificarObservers(EventoSiger::SIMULACION_INICIADA, "", "Simulación reanudada");
}

void SimuladorEntregas::detener() {
    if (!m_corriendo.exchange(false)) return;

    m_pausado.store(false);
    m_condVar.notify_all();

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& hilo : m_hilos) {
        if (hilo.joinable()) {
            hilo.join();
        }
    }
    m_hilos.clear();

    notificarObservers(EventoSiger::SIMULACION_DETENIDA, "", "Simulación detenida");
}

void SimuladorEntregas::setVelocidad(int ms) {
    m_velocidadSimulacion = std::max(100, ms);
}

bool SimuladorEntregas::isCorriendo() const {
    return m_corriendo.load() && !m_pausado.load();
}

EstadisticasSimulacion SimuladorEntregas::getEstadisticas() const {
    return EstadisticasSimulacion {
        m_totalEntregados.load(),
        m_totalFallidos.load(),
        m_enCamino.load()
    };
}

void SimuladorEntregas::hiloRepartidor(std::shared_ptr<Repartidor> rep) {
    while (m_corriendo.load()) {
        std::shared_ptr<Pedido> pedido;

        // Esperar un pedido disponible
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_condVar.wait_for(lock, std::chrono::milliseconds(200), [this] {
                return !m_corriendo.load()
                    || (!m_pausado.load() && !m_colaPedidos->isEmpty());
            });

            if (!m_corriendo.load()) break;
            if (m_pausado.load() || m_colaPedidos->isEmpty()) continue;

            auto optPedido = m_colaPedidos->pop();
            if (!optPedido.has_value()) {
                continue;
            }
            pedido = std::move(optPedido.value());
        }

        if (!pedido) continue;

        try {
            // 1. Marcar EN_CAMINO
            pedido->setEstado(EstadoPedido::EN_CAMINO);
            pedido->setRepartidorId(rep->getId());
            ++m_enCamino;

            notificarObservers(EventoSiger::PEDIDO_EN_CAMINO,
                               pedido->getId(),
                               "EN_CAMINO:" + pedido->getId() + ":" + rep->getId());

            // 2. Simular el viaje con retardo reactivo
            double velKmh = (rep && rep->getVehiculo()) ? rep->getVehiculo()->getVelocidadKmh() : 45.0;
            double minutos = (pedido->getDistanciaKm() / velKmh) * 60.0;
            // Escalar a milisegundos de animación (aprox 1 a 2.5 seg por viaje)
            int duracionMs = std::max(600, static_cast<int>(minutos * (m_velocidadSimulacion / 6.0)));

            int transcurrido = 0;
            while (transcurrido < duracionMs && m_corriendo.load()) {
                while (m_pausado.load() && m_corriendo.load()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                transcurrido += 50;
            }

            // 3. Comprobar si se canceló o detuvo durante el viaje
            if (!m_corriendo.load()) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                notificarObservers(EventoSiger::PEDIDO_CANCELADO,
                                   pedido->getId(),
                                   "PEDIDO_FALLIDO:" + pedido->getId());
                break;
            }

            // 4. Marcar COMPLETADO
            pedido->setEstado(EstadoPedido::COMPLETADO);
            if (pedido->getPaquete()) {
                pedido->getPaquete()->setEstado(EstadoPaquete::ENTREGADO);
            }
            --m_enCamino;
            ++m_totalEntregados;

            notificarObservers(EventoSiger::PEDIDO_COMPLETADO,
                               pedido->getId(),
                               "PEDIDO_COMPLETADO:" + pedido->getId() + ":" + rep->getId());

        } catch (...) {
            if (pedido) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                notificarObservers(EventoSiger::PEDIDO_CANCELADO,
                                   pedido->getId(),
                                   "PEDIDO_FALLIDO:" + pedido->getId());
            }
        }

        m_condVar.notify_all();
    }
}

} // namespace siger
