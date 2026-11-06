#pragma once
/**
 * @file SimuladorEntregas.h
 * @brief Núcleo de simulación multihilo de entregas para SIGER.
 */

#include "Cola.h"
#include "Observer.h"
#include "Pedido.h"
#include "Repartidor.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace siger {

/**
 * @struct EstadisticasSimulacion
 * @brief Instantánea de las métricas de la simulación en curso.
 */
struct EstadisticasSimulacion {
    int entregados { 0 }; ///< Pedidos completados exitosamente.
    int fallidos   { 0 }; ///< Pedidos que finalizaron con error.
    int enCamino   { 0 }; ///< Pedidos actualmente en tránsito.
};

/**
 * @class SimuladorEntregas
 * @brief Motor de simulación multihilo — un hilo por Repartidor.
 */
class SimuladorEntregas : public ISubject, public IObserver {
public:
    explicit SimuladorEntregas(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola);
    ~SimuladorEntregas() override;

    // No copiable ni movible
    SimuladorEntregas(const SimuladorEntregas&)            = delete;
    SimuladorEntregas& operator=(const SimuladorEntregas&) = delete;

    void onEvento(const DatoEvento& evento) override;
    void despertar();

    void agregarRepartidor(std::shared_ptr<Repartidor> repartidor);
    void iniciar();
    void pausar();
    void reanudar();
    void detener();
    void setVelocidad(int ms);

    [[nodiscard]] bool isCorriendo() const;
    [[nodiscard]] EstadisticasSimulacion getEstadisticas() const;

private:
    void hiloRepartidor(std::shared_ptr<Repartidor> rep);

    std::vector<std::shared_ptr<Repartidor>>       m_repartidores;
    std::shared_ptr<Cola<std::shared_ptr<Pedido>>> m_colaPedidos;
    std::vector<std::thread>                       m_hilos;

    std::mutex                                     m_mutex;
    std::condition_variable                        m_condVar;
    std::atomic<bool>                              m_corriendo{ false };
    std::atomic<bool>                              m_pausado  { false };
    int                                            m_velocidadSimulacion{ 1000 };

    std::atomic<int>                               m_totalEntregados{ 0 };
    std::atomic<int>                               m_totalFallidos  { 0 };
    std::atomic<int>                               m_enCamino       { 0 };

    std::mutex                                     m_despachoMutex;
    std::chrono::steady_clock::time_point          m_ultimoDespacho;
};

} // namespace siger
