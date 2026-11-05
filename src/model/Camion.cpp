/**
 * @file Camion.cpp
 */
#include "Camion.h"
#include <sstream>

namespace siger {

Camion::Camion(const std::string& id, const std::string& placa, double capacidadKg)
    : Vehiculo(id, placa, capacidadKg, 30.0)  // velocidad promedio camión: 30 km/h
{}

double Camion::calcularCosto(double distanciaKm, double pesoKg) const {
    return TARIFA_BASE + distanciaKm * TARIFA_KM + pesoKg * TARIFA_KG;
}

std::string Camion::getTipo() const {
    return "Camion";
}

double Camion::calcularTiempoEntrega(double distanciaKm) const {
    return (distanciaKm / m_velocidadKmh) * 60.0;
}

std::string Camion::getDescripcion() const {
    std::ostringstream oss;
    oss << "🚛  Camion [" << m_placa << "]"
        << " | Cap: " << m_capacidadKg << " kg"
        << " | Vel: " << m_velocidadKmh << " km/h"
        << " | Tarifa: S/." << TARIFA_BASE << " + S/." << TARIFA_KM << "/km";
    return oss.str();
}

} // namespace siger
