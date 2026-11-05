/**
 * @file Auto.cpp
 */
#include "Auto.h"
#include <sstream>

namespace siger {

Auto::Auto(const std::string& id, const std::string& placa, double capacidadKg)
    : Vehiculo(id, placa, capacidadKg, 45.0)  // velocidad promedio en ciudad: 45 km/h
{}

double Auto::calcularCosto(double distanciaKm, double pesoKg) const {
    return distanciaKm * TARIFA_KM + pesoKg * TARIFA_KG;
}

std::string Auto::getTipo() const {
    return "Auto";
}

double Auto::calcularTiempoEntrega(double distanciaKm) const {
    return (distanciaKm / m_velocidadKmh) * 60.0;
}

std::string Auto::getDescripcion() const {
    std::ostringstream oss;
    oss << "🚗  Auto [" << m_placa << "]"
        << " | Cap: " << m_capacidadKg << " kg"
        << " | Vel: " << m_velocidadKmh << " km/h"
        << " | Tarifa: S/." << TARIFA_KM << "/km + S/." << TARIFA_KG << "/kg";
    return oss.str();
}

} // namespace siger
