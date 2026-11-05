#pragma once
/**
 * @file Repartidor.h
 * @brief Representa a un repartidor con su vehículo asignado y lista de pedidos.
 *
 * Demuestra: encapsulamiento, uso de smart pointers, composición.
 */
#include "Vehiculo.h"
#include "Pedido.h"
#include <string>
#include <vector>
#include <memory>

namespace siger {

enum class EstadoRepartidor {
    LIBRE,
    EN_REPARTO,
    DESCANSANDO
};

class Repartidor {
public:
    Repartidor(const std::string& id,
               const std::string& nombre,
               std::shared_ptr<Vehiculo> vehiculo);

    // ── Gestión de pedidos ───────────────────────────────────────────
    bool asignarPedido(std::shared_ptr<Pedido> pedido);
    void completarPedidoActual();
    bool tienePedidosPendientes() const;

    // ── Getters ──────────────────────────────────────────────────────
    const std::string&              getId()       const { return m_id; }
    const std::string&              getNombre()   const { return m_nombre; }
    std::shared_ptr<Vehiculo>       getVehiculo() const { return m_vehiculo; }
    EstadoRepartidor                getEstado()   const { return m_estado; }
    int                             getTotalEntregasCompletadas() const { return m_totalCompletadas; }
    const std::vector<std::shared_ptr<Pedido>>& getPedidosPendientes() const;

    std::string getEstadoStr() const;

private:
    std::string                        m_id;
    std::string                        m_nombre;
    std::shared_ptr<Vehiculo>          m_vehiculo;
    EstadoRepartidor                   m_estado { EstadoRepartidor::LIBRE };
    std::vector<std::shared_ptr<Pedido>> m_pedidosPendientes;
    int                                m_totalCompletadas { 0 };
};

} // namespace siger
