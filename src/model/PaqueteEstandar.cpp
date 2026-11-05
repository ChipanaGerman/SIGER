/**
 * @file PaqueteEstandar.cpp
 */
#include "PaqueteEstandar.h"
#include <algorithm>

namespace siger {

PaqueteEstandar::PaqueteEstandar(const std::string& id,
                                 const std::string& descripcion,
                                 double pesoKg,
                                 const std::string& direccionDestino,
                                 int diasEntrega)
    : Paquete(id, descripcion, pesoKg, direccionDestino)
    , m_diasEntrega(diasEntrega)
{}

int PaqueteEstandar::calcularPrioridad() const {
    // Prioridad: a mayor peso y menor plazo, mayor prioridad (1-10)
    int prioridad = 5;
    if (m_pesoKg > 20.0) prioridad += 2;
    if (m_diasEntrega <= 1) prioridad += 3;
    else if (m_diasEntrega <= 2) prioridad += 1;
    return std::min(10, prioridad);
}

std::string PaqueteEstandar::getTipo() const {
    return "Estándar";
}

bool PaqueteEstandar::requiereManejoEspecial() const {
    return false;
}

} // namespace siger
