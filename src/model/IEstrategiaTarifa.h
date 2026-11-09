#pragma once
/**
 * @file IEstrategiaTarifa.h
 * @brief Patrón de diseño Strategy (GoF) para el cálculo dinámico de tarifas de entrega en SIGER.
 *
 * Permite desacoplar los algoritmos de cálculo de precio de la clase Pedido y Vehículo,
 * facilitando la incorporación de nuevas políticas comerciales (Estándar, Frágil, Carga Pesada).
 *
 * @author SIGER Dev Team
 */

#include <string>
#include <algorithm>
#include <cmath>

namespace siger {

/**
 * @interface IEstrategiaTarifa
 * @brief Interfaz base del patrón Strategy para políticas de tarificación en Soles (S/.).
 */
class IEstrategiaTarifa {
public:
    virtual ~IEstrategiaTarifa() = default;

    /**
     * @brief Calcula la tarifa de envío en Soles.
     * @param distanciaKm Distancia vial real de conducción en kilómetros.
     * @param pesoKg Peso total de la mercancía transportada en kilogramos.
     * @param esFragil Indica si requiere embalaje de seguridad y manipulación especial.
     * @return Importe total a cobrar en Soles (S/.).
     */
    virtual double calcularTarifa(double distanciaKm, double pesoKg, bool esFragil) const = 0;

    /**
     * @brief Devuelve el nombre comercial de la estrategia de cobro.
     */
    virtual std::string getNombre() const = 0;
};

/**
 * @class TarifaEstandarStrategy
 * @brief Estrategia para entregas urbanas convencionales (Motos y Autos ligeros).
 *
 * Tarifa base: S/. 4.00
 * Costo por km: S/. 1.20 / km
 * Recargo por peso excedente (> 5kg): S/. 0.50 / kg
 * Recargo por mercancía frágil: + S/. 3.50
 */
class TarifaEstandarStrategy : public IEstrategiaTarifa {
public:
    double calcularTarifa(double distanciaKm, double pesoKg, bool esFragil) const override {
        double costo = 4.00 + (distanciaKm * 1.20);
        if (pesoKg > 5.0) {
            costo += (pesoKg - 5.0) * 0.50;
        }
        if (esFragil) {
            costo += 3.50;
        }
        return std::round(costo * 10.0) / 10.0;
    }

    std::string getNombre() const override {
        return "Tarifa Urbana Estándar";
    }
};

/**
 * @class TarifaFragilStrategy
 * @brief Estrategia especializada en paquetería delicada con póliza de seguro y cuidado prioritario.
 *
 * Tarifa base: S/. 7.50
 * Costo por km: S/. 1.60 / km
 * Seguro de protección: S/. 5.00
 */
class TarifaFragilStrategy : public IEstrategiaTarifa {
public:
    double calcularTarifa(double distanciaKm, double pesoKg, bool esFragil) const override {
        double costo = 7.50 + (distanciaKm * 1.60);
        if (pesoKg > 5.0) {
            costo += (pesoKg - 5.0) * 0.70;
        }
        // Seguro prioritario obligatorio
        costo += 5.00;
        return std::round(costo * 10.0) / 10.0;
    }

    std::string getNombre() const override {
        return "Tarifa Protección Especial Frágil";
    }
};

/**
 * @class TarifaPesadaStrategy
 * @brief Estrategia para transporte de alto tonelaje e industrial (Camiones de reparto).
 *
 * Tarifa base: S/. 14.00
 * Costo por km: S/. 2.20 / km
 * Tarifa de carga pesada: S/. 0.25 por cada kg sobre 50 kg
 */
class TarifaPesadaStrategy : public IEstrategiaTarifa {
public:
    double calcularTarifa(double distanciaKm, double pesoKg, bool esFragil) const override {
        double costo = 14.00 + (distanciaKm * 2.20);
        if (pesoKg > 50.0) {
            costo += (pesoKg - 50.0) * 0.25;
        }
        if (esFragil) {
            costo += 6.00;
        }
        return std::round(costo * 10.0) / 10.0;
    }

    std::string getNombre() const override {
        return "Tarifa Logística Pesada / Industrial";
    }
};

} // namespace siger
