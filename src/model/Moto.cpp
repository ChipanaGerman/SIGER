/**
 * @file Moto.cpp
 * @brief Implementación de la clase Moto.
 */
#include "Moto.h"
#include <sstream>
#include <algorithm>

namespace siger {

Moto::Moto(const std::string& id, const std::string& placa, double capacidadKg)
    : Vehiculo(id, placa, capacidadKg, 60.0)  // velocidad promedio moto: 60 km/h
{}

double Moto::calcularCosto(double distanciaKm, double pesoKg) const {
    double costo = distanciaKm * TARIFA_KM;
    // Recargo si el peso supera 5 kg
    double pesoExtra = std::max(0.0, pesoKg - 5.0);
    costo += pesoExtra * RECARGO_KG;
    return costo;
}

std::string Moto::getTipo() const {
    return "Moto";
}

double Moto::calcularTiempoEntrega(double distanciaKm) const {
    // tiempo (minutos) = distancia / velocidad * 60
    return (distanciaKm / m_velocidadKmh) * 60.0;
}

std::string Moto::getDescripcion() const {
    std::ostringstream oss;
    oss << "🏍️  Moto [" << m_placa << "]"
        << " | Cap: " << m_capacidadKg << " kg"
        << " | Vel: " << m_velocidadKmh << " km/h"
        << " | Tarifa: S/." << TARIFA_KM << "/km";
    return oss.str();
}

} // namespace siger
