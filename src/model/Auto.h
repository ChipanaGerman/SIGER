#pragma once
/**
 * @file Auto.h
 * @brief Vehículo tipo Auto — capacidad media, costo medio.
 */
#include "Vehiculo.h"

namespace siger {

class Auto : public Vehiculo {
public:
    Auto(const std::string& id,
         const std::string& placa,
         double capacidadKg = 100.0);

    double      calcularCosto(double distanciaKm, double pesoKg) const override;
    std::string getTipo()                                        const override;
    double      calcularTiempoEntrega(double distanciaKm)        const override;
    std::string getDescripcion()                                 const override;

private:
    static constexpr double TARIFA_KM  = 1.50;
    static constexpr double TARIFA_KG  = 0.30;
};

} // namespace siger
