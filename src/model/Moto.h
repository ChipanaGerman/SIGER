#pragma once
/**
 * @file Moto.h
 * @brief Vehículo tipo Moto — rápida, baja capacidad, costo bajo.
 *
 * Demuestra: herencia, polimorfismo, sobreescritura de virtuales.
 */
#include "Vehiculo.h"

namespace siger {

class Moto : public Vehiculo {
public:
    /**
     * @param id          Identificador único.
     * @param placa       Placa del vehículo.
     * @param capacidadKg Capacidad máxima en kg (default 15 kg).
     */
    Moto(const std::string& id,
         const std::string& placa,
         double capacidadKg = 15.0);

    double      calcularCosto(double distanciaKm, double pesoKg) const override;
    std::string getTipo()                                        const override;
    double      calcularTiempoEntrega(double distanciaKm)        const override;
    std::string getDescripcion()                                 const override;

private:
    // Tarifa base por km para moto (S/. por km)
    static constexpr double TARIFA_KM = 0.80;
    // Recargo por kg extra sobre los 5 primeros
    static constexpr double RECARGO_KG = 0.50;
};

} // namespace siger
