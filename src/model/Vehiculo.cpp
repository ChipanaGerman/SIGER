/**
 * @file Vehiculo.cpp
 * @brief Implementación de la clase base abstracta Vehiculo.
 */
#include "Vehiculo.h"
#include <sstream>

namespace siger {

Vehiculo::Vehiculo(const std::string& id,
                   const std::string& placa,
                   double capacidadKg,
                   double velocidadKmh)
    : m_id(id)
    , m_placa(placa)
    , m_capacidadKg(capacidadKg)
    , m_velocidadKmh(velocidadKmh)
{}

std::string Vehiculo::getDescripcion() const {
    std::ostringstream oss;
    oss << getTipo()
        << " [" << m_placa << "]"
        << " Cap:" << m_capacidadKg << "kg"
        << " Vel:" << m_velocidadKmh << "km/h";
    return oss.str();
}

bool Vehiculo::puedeCargar(double pesoKg) const {
    return pesoKg > 0.0 && pesoKg <= m_capacidadKg;
}

} // namespace siger
