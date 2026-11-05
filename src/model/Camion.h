#pragma once
/**
 * @file Camion.h
 * @brief Vehículo tipo Camión — alta capacidad, mayor costo, más lento.
 */
#include "Vehiculo.h"

namespace siger {

class Camion : public Vehiculo {
public:
    Camion(const std::string& id,
           const std::string& placa,
           double capacidadKg = 2000.0);

    double      calcularCosto(double distanciaKm, double pesoKg) const override;
    std::string getTipo()                                        const override;
    double      calcularTiempoEntrega(double distanciaKm)        const override;
    std::string getDescripcion()                                 const override;

private:
    static constexpr double TARIFA_BASE = 5.00;  // costo fijo de salida
    static constexpr double TARIFA_KM   = 2.50;
    static constexpr double TARIFA_KG   = 0.10;
};

} // namespace siger
