#pragma once
/**
 * @file GestorPedidos.h
 * @brief Gestor centralizado de pedidos para SIGER.
 */

#include "Cola.h"
#include "Observer.h"
#include "Pedido.h"
#include "Repositorio.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace siger {

/**
 * @struct EstadisticasPedidos
 * @brief Instantánea de las métricas del gestor de pedidos.
 */
struct EstadisticasPedidos {
    int total      { 0 }; ///< Total de pedidos registrados.
    int pendientes { 0 }; ///< Pedidos en cola o activos (aún no finalizados).
    int completados{ 0 }; ///< Pedidos finalizados correctamente.
    int cancelados { 0 }; ///< Pedidos anulados.
    double totalFacturado { 0.0 }; ///< Importe total facturado en Soles (S/.)
};

/**
 * @class GestorPedidos
 * @brief Controlador del ciclo de vida de los pedidos de entrega.
 *
 * Implementa ISubject para notificar eventos a la GUI, e IObserver
 * para recibir notificaciones del SimuladorEntregas cuando un pedido se completa.
 */
class GestorPedidos : public ISubject, public IObserver {
public:
    explicit GestorPedidos(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola = nullptr);
    ~GestorPedidos() override = default;

    // No copiable ni movible
    GestorPedidos(const GestorPedidos&)            = delete;
    GestorPedidos& operator=(const GestorPedidos&) = delete;

    [[nodiscard]] std::shared_ptr<Cola<std::shared_ptr<Pedido>>> getColaEspera() const {
        return m_colaEspera;
    }

    // Implementación de IObserver (escucha a SimuladorEntregas)
    void onEvento(const DatoEvento& evento) override;

    bool registrarPedido(std::shared_ptr<Pedido> pedido);
    bool cancelarPedido(const std::string& id);
    void marcarCompletado(const std::string& id);
    [[nodiscard]] std::optional<std::shared_ptr<Pedido>> obtenerSiguiente();
    [[nodiscard]] std::optional<std::shared_ptr<Pedido>> obtenerPedido(const std::string& id) const;
    [[nodiscard]] std::vector<std::shared_ptr<Pedido>> getPedidosActivos() const;
    [[nodiscard]] EstadisticasPedidos getEstadisticas() const;

private:
    Repositorio<Pedido>                           m_repositorioPedidos;
    std::shared_ptr<Cola<std::shared_ptr<Pedido>>> m_colaEspera;
    std::atomic<int>                              m_pedidosCompletados{ 0 };
    std::atomic<int>                              m_pedidosCancelados { 0 };
    /// @brief Conjunto de IDs ya contabilizados como completados (evita doble conteo).
    std::unordered_set<std::string>               m_contabilizados;
    mutable std::mutex                            m_contMutex;
};

} // namespace siger
