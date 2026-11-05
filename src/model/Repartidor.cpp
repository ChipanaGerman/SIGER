/**
 * @file Repartidor.cpp
 */
#include "Repartidor.h"
#include <stdexcept>

namespace siger {

Repartidor::Repartidor(const std::string& id,
                       const std::string& nombre,
                       std::shared_ptr<Vehiculo> vehiculo)
    : m_id(id)
    , m_nombre(nombre)
    , m_vehiculo(std::move(vehiculo))
{}

bool Repartidor::asignarPedido(std::shared_ptr<Pedido> pedido) {
    if (!pedido) return false;
    // Verificar que el vehículo puede cargar el peso del paquete
    if (m_vehiculo && !m_vehiculo->puedeCargar(pedido->getPaquete()->getPesoKg())) {
        return false;
    }
    m_pedidosPendientes.push_back(std::move(pedido));
    m_estado = EstadoRepartidor::EN_REPARTO;
    return true;
}

void Repartidor::completarPedidoActual() {
    if (m_pedidosPendientes.empty()) return;
    auto& pedido = m_pedidosPendientes.front();
    pedido->setEstado(EstadoPedido::COMPLETADO);
    pedido->getPaquete()->setEstado(EstadoPaquete::ENTREGADO);
    m_pedidosPendientes.erase(m_pedidosPendientes.begin());
    ++m_totalCompletadas;
    if (m_pedidosPendientes.empty()) {
        m_estado = EstadoRepartidor::LIBRE;
    }
}

bool Repartidor::tienePedidosPendientes() const {
    return !m_pedidosPendientes.empty();
}

const std::vector<std::shared_ptr<Pedido>>& Repartidor::getPedidosPendientes() const {
    return m_pedidosPendientes;
}

std::string Repartidor::getEstadoStr() const {
    switch (m_estado) {
        case EstadoRepartidor::LIBRE:        return "LIBRE";
        case EstadoRepartidor::EN_REPARTO:   return "EN REPARTO";
        case EstadoRepartidor::DESCANSANDO:  return "DESCANSANDO";
        default:                             return "DESCONOCIDO";
    }
}

} // namespace siger
