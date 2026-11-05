/**
 * @file Paquete.cpp
 */
#include "Paquete.h"

namespace siger {

Paquete::Paquete(const std::string& id,
                 const std::string& descripcion,
                 double pesoKg,
                 const std::string& direccionDestino)
    : m_id(id)
    , m_descripcion(descripcion)
    , m_pesoKg(pesoKg)
    , m_direccionDestino(direccionDestino)
{}

std::string Paquete::getEstadoStr() const {
    switch (m_estado) {
        case EstadoPaquete::PENDIENTE:    return "PENDIENTE";
        case EstadoPaquete::ASIGNADO:     return "ASIGNADO";
        case EstadoPaquete::EN_TRANSITO:  return "EN TRÁNSITO";
        case EstadoPaquete::ENTREGADO:    return "ENTREGADO";
        case EstadoPaquete::CANCELADO:    return "CANCELADO";
        default:                          return "DESCONOCIDO";
    }
}

} // namespace siger
