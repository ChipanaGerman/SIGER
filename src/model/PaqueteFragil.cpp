/**
 * @file PaqueteFragil.cpp
 */
#include "PaqueteFragil.h"
#include <algorithm>

namespace siger {

PaqueteFragil::PaqueteFragil(const std::string& id,
                             const std::string& descripcion,
                             double pesoKg,
                             const std::string& direccionDestino,
                             double valorAsegurado)
    : Paquete(id, descripcion, pesoKg, direccionDestino)
    , m_valorAsegurado(valorAsegurado)
{}

int PaqueteFragil::calcularPrioridad() const {
    // Paquetes frágiles siempre tienen prioridad mínima 7
    int prioridad = 7;
    if (m_valorAsegurado > 1000.0) prioridad += 2;
    if (m_pesoKg < 2.0)           prioridad += 1;  // frágil y pequeño = máx cuidado
    return std::min(10, prioridad);
}

std::string PaqueteFragil::getTipo() const {
    return "Frágil";
}

bool PaqueteFragil::requiereManejoEspecial() const {
    return true;  // siempre requiere manejo especial
}

} // namespace siger
