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
#include <optional>
#include <string>
#include <vector>

namespace siger {

/**
 * @struct EstadisticasPedidos
 * @brief Instantánea de las métricas del gestor de pedidos.
 */
struct EstadisticasPedidos {
    int total      { 0 }; ///< Total de pedidos registrados.
    int pendientes { 0 }; ///< Pedidos en cola (aún no despachados).
    int completados{ 0 }; ///< Pedidos finalizados correctamente.
    int cancelados { 0 }; ///< Pedidos anulados.
};

/**
 * @class GestorPedidos
 * @brief Controlador del ciclo de vida de los pedidos de entrega.
 */
class GestorPedidos : public ISubject {
public:
    explicit GestorPedidos(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola = nullptr);
    ~GestorPedidos() = default;

    // No copiable ni movible
    GestorPedidos(const GestorPedidos&)            = delete;
    GestorPedidos& operator=(const GestorPedidos&) = delete;

    [[nodiscard]] std::shared_ptr<Cola<std::shared_ptr<Pedido>>> getColaEspera() const {
        return m_colaEspera;
    }

    bool registrarPedido(std::shared_ptr<Pedido> pedido);
    bool cancelarPedido(const std::string& id);
    [[nodiscard]] std::optional<std::shared_ptr<Pedido>> obtenerSiguiente();
    [[nodiscard]] std::optional<std::shared_ptr<Pedido>> obtenerPedido(const std::string& id) const;
    [[nodiscard]] std::vector<std::shared_ptr<Pedido>> getPedidosActivos() const;
    [[nodiscard]] EstadisticasPedidos getEstadisticas() const;

private:
    Repositorio<Pedido>                           m_repositorioPedidos;
    std::shared_ptr<Cola<std::shared_ptr<Pedido>>> m_colaEspera;
    std::atomic<int>                              m_pedidosCompletados{ 0 };
    std::atomic<int>                              m_pedidosCancelados { 0 };
};

} // namespace siger
