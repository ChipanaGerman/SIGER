/**
 * @file SimuladorEntregas.cpp
 * @brief Implementación de SimuladorEntregas — motor multihilo de SIGER.
 */

#include "SimuladorEntregas.h"
#include "ServicioRutas.h"
#include <QString>

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
    m_ultimoDespacho = std::chrono::steady_clock::time_point{};

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& rep : m_repartidores) {
            m_hilos.emplace_back(&SimuladorEntregas::hiloRepartidor, this, rep);
        }
    }

    m_condVar.notify_all();

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

    std::vector<std::thread> hilosLocales;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        hilosLocales = std::move(m_hilos);
        m_hilos.clear();
    }

    // Unirse a los hilos FUERA del lock para evitar cualquier deadlock
    for (auto& hilo : hilosLocales) {
        if (hilo.joinable()) {
            hilo.join();
        }
    }

    notificarObservers(EventoSiger::SIMULACION_DETENIDA, "", "Simulación detenida");
}

void SimuladorEntregas::setVelocidad(int ms) {
    m_velocidadSimulacion = std::max(1, ms);
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
    if (!rep || !rep->getVehiculo()) return;
    const std::string tipoVehiculo = rep->getVehiculo()->getTipo();
    const double capVehiculo       = rep->getVehiculo()->getCapacidadKg();

    while (m_corriendo.load()) {
        std::shared_ptr<Pedido> pedido;

        // Esperar un pedido disponible compatible con el vehículo de este repartidor
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            m_condVar.wait_for(lock, std::chrono::milliseconds(200), [this, &tipoVehiculo, capVehiculo] {
                if (!m_corriendo.load()) return true;
                if (m_pausado.load() || m_colaPedidos->isEmpty()) return false;

                // Solo despertar si hay un pedido en cola que este vehículo pueda atender
                return m_colaPedidos->contieneSi([&tipoVehiculo, capVehiculo](const std::shared_ptr<Pedido>& p) {
                    if (!p || p->getEstado() == EstadoPedido::FALLIDO || p->getEstado() == EstadoPedido::COMPLETADO) {
                        return false;
                    }
                    return p->esCompatibleCon(tipoVehiculo, capVehiculo);
                });
            });

            if (!m_corriendo.load()) break;
            if (m_pausado.load() || m_colaPedidos->isEmpty()) continue;

            // Extraer el primer pedido compatible con este vehículo
            auto optPedido = m_colaPedidos->extraerSi([&tipoVehiculo, capVehiculo](const std::shared_ptr<Pedido>& p) {
                if (!p || p->getEstado() == EstadoPedido::FALLIDO || p->getEstado() == EstadoPedido::COMPLETADO) {
                    return false;
                }
                return p->esCompatibleCon(tipoVehiculo, capVehiculo);
            });

            if (!optPedido.has_value()) {
                // Ningún pedido en la cola es para este tipo de vehículo en este momento.
                // El repartidor permanece en espera hasta que llegue uno o se despierte.
                continue;
            }
            pedido = std::move(optPedido.value());

            // Notificar inmediatamente a otros hilos repartidores para que tomen pedidos en paralelo
            m_condVar.notify_all();
        }

        if (!pedido) continue;

        try {
            // Escalonamiento de salidas para evitar que salgan todos al mismo instante
            {
                std::unique_lock<std::mutex> despLock(m_despachoMutex);
                auto ahora = std::chrono::steady_clock::now();
                auto transcurridoMs = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_ultimoDespacho).count();
                // A velocidad de prueba unitaria (ms < 100), no retardamos
                int delayEscalon = (m_velocidadSimulacion < 100) ? 0 : 700;
                if (transcurridoMs < delayEscalon && m_ultimoDespacho.time_since_epoch().count() > 0) {
                    int espera = delayEscalon - static_cast<int>(transcurridoMs);
                    std::this_thread::sleep_for(std::chrono::milliseconds(espera));
                }
                m_ultimoDespacho = std::chrono::steady_clock::now();
            }

            // 1. Marcar EN_CAMINO y asociar al repartidor
            pedido->setEstado(EstadoPedido::EN_CAMINO);
            pedido->setRepartidorId(rep->getId());
            rep->asignarPedido(pedido);
            ++m_enCamino;

            notificarObservers(EventoSiger::PEDIDO_EN_CAMINO,
                               pedido->getId(),
                               "EN_CAMINO:" + pedido->getId() + ":" + rep->getId() + ":" + tipoVehiculo);

            // 2. Simular el viaje con duración sincronizada y proporcional a la distancia real (OSRM)
            double distKm = pedido->getDistanciaKm();
            if (distKm <= 0.2) {
                distKm = ServicioRutas::instancia().calcularDistanciaKm(
                    QString::fromStdString(pedido->getOrigen()),
                    QString::fromStdString(pedido->getDestino())
                );
            }
            if (distKm <= 0.2) distKm = 2.0;

            double segs = 4.0;
            if (tipoVehiculo == "Moto") {
                segs = 4.0 + distKm * 1.0;
            } else if (tipoVehiculo == "Camion" || tipoVehiculo == "Camión") {
                segs = 6.0 + distKm * 1.6;
            } else {
                segs = 5.0 + distKm * 1.3;
            }

            // Margen de 400ms para asegurar que el móvil gráfico llega al pin
            // y se asienta visualmente antes de emitir la finalización del pedido
            int duracionBaseMs = static_cast<int>((segs + 0.4) * 1000.0);
            double factorVelocidad = static_cast<double>(m_velocidadSimulacion) / 1000.0;
            int duracionMs = (m_velocidadSimulacion < 100)
                             ? 10
                             : std::max(60, static_cast<int>(duracionBaseMs * factorVelocidad));

            int transcurrido = 0;
            int paso = std::min(50, duracionMs);
            while (transcurrido < duracionMs && m_corriendo.load()) {
                while (m_pausado.load() && m_corriendo.load()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(paso));
                transcurrido += paso;
            }

            // 3. Comprobar si se canceló o detuvo durante el viaje
            if (!m_corriendo.load()) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                rep->completarPedidoActual();
                notificarObservers(EventoSiger::PEDIDO_CANCELADO,
                                   pedido->getId(),
                                   "PEDIDO_FALLIDO:" + pedido->getId());
                break;
            }

            // 4. Marcar COMPLETADO y liberar al repartidor para su siguiente entrega
            pedido->setEstado(EstadoPedido::COMPLETADO);
            if (pedido->getPaquete()) {
                pedido->getPaquete()->setEstado(EstadoPaquete::ENTREGADO);
            }
            --m_enCamino;
            ++m_totalEntregados;
            rep->completarPedidoActual();

            notificarObservers(EventoSiger::PEDIDO_COMPLETADO,
                               pedido->getId(),
                               "PEDIDO_COMPLETADO:" + pedido->getId() + ":" + rep->getId());

        } catch (...) {
            if (pedido) {
                pedido->setEstado(EstadoPedido::FALLIDO);
                --m_enCamino;
                ++m_totalFallidos;
                rep->completarPedidoActual();
                notificarObservers(EventoSiger::PEDIDO_CANCELADO,
                                   pedido->getId(),
                                   "PEDIDO_FALLIDO:" + pedido->getId());
            }
        }

        // Despertar a otros hilos repartidores: una unidad ha regresado
        // y puede haber pedidos pendientes en espera de su tipo de vehículo
        m_condVar.notify_all();

        // Pequeño descanso voluntario (50ms) para ceder turno y asegurar balanceo entre conductores
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

} // namespace siger
