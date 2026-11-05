#pragma once
/**
 * @file   SimuladorEntregas.h
 * @brief  Núcleo de simulación multihilo de entregas para SIGER.
 *
 * SimuladorEntregas gestiona un pool de std::thread (uno por Repartidor),
 * sincronizados mediante std::mutex + std::condition_variable para el
 * modelo productor/consumidor.  Hereda de ISubject para notificar a la GUI
 * y capas superiores sobre eventos de simulación.
 *
 * Patrón demostrado: Observer + productor-consumidor con hilos nativos.
 *
 * @author SIGER Dev Team
 */

#include "Cola.h"
#include "Observer.h"
#include "Pedido.h"
#include "Repartidor.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace siger {

// ─────────────────────────────────────────────────────────────────────────────
/**
 * @struct EstadisticasSimulacion
 * @brief  Instantánea de las métricas de la simulación en curso.
 */
struct EstadisticasSimulacion {
    int entregados { 0 }; ///< Pedidos completados exitosamente.
    int fallidos   { 0 }; ///< Pedidos que finalizaron con error.
    int enCamino   { 0 }; ///< Pedidos actualmente en tránsito.
};

// ─────────────────────────────────────────────────────────────────────────────
/**
 * @class  SimuladorEntregas
 * @brief  Motor de simulación multihilo — un hilo por Repartidor.
 *
 * Flujo de vida:
 *  1. Construir con una Cola<shared_ptr<Pedido>> compartida con GestorPedidos.
 *  2. Registrar repartidores con agregarRepartidor().
 *  3. Llamar a iniciar() para lanzar los hilos de trabajo.
 *  4. pausar() / reanudar() controlan la ejecución sin destruir hilos.
 *  5. detener() señaliza el fin y hace join() de todos los hilos.
 *
 * La clase hereda ISubject para propagar EventoSiger a los observadores GUI.
 */
class SimuladorEntregas : public ISubject {
public:
    // ── Constructor / Destructor ──────────────────────────────────────────────

    /**
     * @brief Construye el simulador con la cola de pedidos compartida.
     * @param cola  Cola thread-safe compartida con GestorPedidos.
     */
    explicit SimuladorEntregas(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola);

    /**
     * @brief Destructor: detiene y hace join de todos los hilos activos.
     */
    ~SimuladorEntregas();

    // No copiable ni movible (contiene threads y atomics)
    SimuladorEntregas(const SimuladorEntregas&)            = delete;
    SimuladorEntregas& operator=(const SimuladorEntregas&) = delete;
    SimuladorEntregas(SimuladorEntregas&&)                 = delete;
    SimuladorEntregas& operator=(SimuladorEntregas&&)      = delete;

    // ── Gestión de repartidores ───────────────────────────────────────────────

    /**
     * @brief Registra un repartidor que participará en la simulación.
     *
     * Debe llamarse antes de iniciar().  Agregar repartidores después de
     * iniciar() es hilo-seguro pero el hilo se lanza de inmediato.
     *
     * @param repartidor  Puntero compartido al repartidor.
     */
    void agregarRepartidor(std::shared_ptr<Repartidor> repartidor);

    // ── Control de la simulación ──────────────────────────────────────────────

    /**
     * @brief Lanza un hilo de trabajo por cada repartidor registrado.
     *
     * Idempotente: si ya está corriendo, no hace nada.
     */
    void iniciar();

    /**
     * @brief Suspende los hilos de trabajo sin terminarlos.
     *
     * Los hilos bloquean en la condition_variable hasta reanudar().
     */
    void pausar();

    /**
     * @brief Reanuda la simulación previamente pausada.
     */
    void reanudar();

    /**
     * @brief Detiene la simulación y espera a que todos los hilos terminen.
     *
     * Tras detener(), el objeto puede destruirse de forma segura.
     */
    void detener();

    // ── Configuración ─────────────────────────────────────────────────────────

    /**
     * @brief Ajusta la velocidad de simulación (ms por unidad de distancia).
     * @param ms  Milisegundos por kilómetro de trayecto (mínimo 100 ms).
     */
    void setVelocidad(int ms);

    // ── Consultas ─────────────────────────────────────────────────────────────

    /**
     * @brief Indica si la simulación está actualmente en ejecución.
     * @return true si los hilos están corriendo y no pausados.
     */
    [[nodiscard]] bool isCorriendo() const;

    /**
     * @brief Devuelve una instantánea de las estadísticas de la simulación.
     * @return EstadisticasSimulacion con conteos actuales.
     */
    [[nodiscard]] EstadisticasSimulacion getEstadisticas() const;

private:
    // ── Función de hilo ───────────────────────────────────────────────────────

    /**
     * @brief Función ejecutada por cada hilo de repartidor.
     *
     * Ciclo productor-consumidor:
     *  1. Espera en m_condVar mientras pausado o cola vacía.
     *  2. Extrae un Pedido de la cola.
     *  3. Actualiza estado a EN_CAMINO y notifica observers.
     *  4. Simula el viaje con sleep proporcional a distanciaKm.
     *  5. Marca el pedido COMPLETADO e incrementa m_totalEntregados.
     *
     * @param rep  Repartidor propietario del hilo.
     */
    void hiloRepartidor(std::shared_ptr<Repartidor> rep);

    // ── Miembros ──────────────────────────────────────────────────────────────

    /// Cola de pedidos compartida con GestorPedidos (productor externo).
    std::shared_ptr<Cola<std::shared_ptr<Pedido>>> m_colaPedidos;

    /// Repartidores registrados (uno por hilo).
    std::vector<std::shared_ptr<Repartidor>> m_repartidores;

    /// Hilos de trabajo activos (uno por repartidor).
    std::vector<std::thread> m_hilos;

    /// Mutex que protege m_hilos, m_repartidores y el acceso a la cola.
    std::mutex m_mutex;

    /// Condition variable para el modelo productor-consumidor y pausa.
    std::condition_variable m_condVar;

    /// true mientras la simulación está activa (hilos en ejecución).
    std::atomic<bool> m_corriendo { false };

    /// true mientras la simulación está pausada.
    std::atomic<bool> m_pausado { false };

    /// Contador de pedidos actualmente en tránsito (EN_CAMINO).
    std::atomic<int> m_enCamino { 0 };

    /// Contador total de pedidos completados exitosamente.
    std::atomic<int> m_totalEntregados { 0 };

    /// Contador total de pedidos que finalizaron con error.
    std::atomic<int> m_totalFallidos { 0 };

    /// Milisegundos que dura la simulación por kilómetro de distancia.
    int m_velocidadSimulacion { 1000 };
};

} // namespace siger
