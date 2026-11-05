/**
 * @file Pedido.cpp
 */
#include "Pedido.h"

namespace siger {

Pedido::Pedido(const std::string& id,
               std::shared_ptr<Paquete> paquete,
               const std::string& origen,
               const std::string& destino,
               double distanciaKm)
    : m_id(id)
    , m_paquete(std::move(paquete))
    , m_origen(origen)
    , m_destino(destino)
    , m_distanciaKm(distanciaKm)
{}

std::string Pedido::getEstadoStr() const {
    switch (m_estado) {
        case EstadoPedido::NUEVO:       return "NUEVO";
        case EstadoPedido::ASIGNADO:    return "ASIGNADO";
        case EstadoPedido::EN_CAMINO:   return "EN CAMINO";
        case EstadoPedido::COMPLETADO:  return "COMPLETADO";
        case EstadoPedido::FALLIDO:     return "FALLIDO";
        default:                        return "DESCONOCIDO";
    }
}

int Pedido::getPrioridad() const {
    return m_paquete ? m_paquete->calcularPrioridad() : 0;
}

} // namespace siger
