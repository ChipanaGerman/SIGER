/**
 * @file VehiculoFactory.cpp
 * @brief Implementación de las fábricas concretas y del registro FactoriaVehiculos.
 *
 * Contiene la lógica de cada Factory Method concreto (MotoFactory, AutoFactory,
 * CamionFactory) y la implementación completa del registro estático
 * FactoriaVehiculos, incluyendo la auto-inicialización de los tres tipos
 * estándar mediante @c registrarDefaults().
 *
 * @namespace siger
 */

#include "VehiculoFactory.h"

#include <stdexcept>

namespace siger {

// ─────────────────────────────────────────────────────────────────────────────
// Definición del miembro estático
// ─────────────────────────────────────────────────────────────────────────────

/// @brief Registro estático indexado por nombre de tipo de vehículo.
std::map<std::string, std::unique_ptr<VehiculoFactory>> FactoriaVehiculos::s_registro;

// ─────────────────────────────────────────────────────────────────────────────
// MotoFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Crea una nueva Moto con los parámetros dados.
 */
std::shared_ptr<Vehiculo> MotoFactory::crearVehiculo(
    const std::string& id,
    const std::string& placa,
    double             capacidadKg) const
{
    return std::make_shared<Moto>(id, placa, capacidadKg);
}

/**
 * @brief Devuelve "Moto".
 */
std::string MotoFactory::getTipoVehiculo() const
{
    return "Moto";
}

// ─────────────────────────────────────────────────────────────────────────────
// AutoFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Crea un nuevo Auto con los parámetros dados.
 */
std::shared_ptr<Vehiculo> AutoFactory::crearVehiculo(
    const std::string& id,
    const std::string& placa,
    double             capacidadKg) const
{
    return std::make_shared<Auto>(id, placa, capacidadKg);
}

/**
 * @brief Devuelve "Auto".
 */
std::string AutoFactory::getTipoVehiculo() const
{
    return "Auto";
}

// ─────────────────────────────────────────────────────────────────────────────
// CamionFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Crea un nuevo Camion con los parámetros dados.
 */
std::shared_ptr<Vehiculo> CamionFactory::crearVehiculo(
    const std::string& id,
    const std::string& placa,
    double             capacidadKg) const
{
    return std::make_shared<Camion>(id, placa, capacidadKg);
}

/**
 * @brief Devuelve "Camion".
 */
std::string CamionFactory::getTipoVehiculo() const
{
    return "Camion";
}

// ─────────────────────────────────────────────────────────────────────────────
// FactoriaVehiculos — Registry
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Registra una fábrica bajo el nombre de tipo indicado.
 *
 * Si ya existe una entrada para ese tipo, la sobreescribe.
 *
 * @param tipo    Nombre del tipo de vehículo.
 * @param fabrica unique_ptr a la fábrica concreta.
 */
void FactoriaVehiculos::registrar(const std::string&           tipo,
                                  std::unique_ptr<VehiculoFactory> fabrica)
{
    s_registro[tipo] = std::move(fabrica);
}

/**
 * @brief Crea un vehículo usando la fábrica registrada para @p tipo.
 *
 * @throws std::invalid_argument si @p tipo no tiene fábrica registrada.
 */
std::shared_ptr<Vehiculo> FactoriaVehiculos::crear(
    const std::string& tipo,
    const std::string& id,
    const std::string& placa,
    double             capacidadKg)
{
    auto it = s_registro.find(tipo);
    if (it == s_registro.end()) {
        throw std::invalid_argument(
            "FactoriaVehiculos::crear — tipo no registrado: \"" + tipo + "\"");
    }
    return it->second->crearVehiculo(id, placa, capacidadKg);
}

/**
 * @brief Registra las tres fábricas estándar del sistema (Moto, Auto, Camion).
 *
 * Es idempotente: si ya existen entradas no las duplica.
 */
void FactoriaVehiculos::registrarDefaults()
{
    if (s_registro.find("Moto") == s_registro.end()) {
        s_registro["Moto"] = std::make_unique<MotoFactory>();
    }
    if (s_registro.find("Auto") == s_registro.end()) {
        s_registro["Auto"] = std::make_unique<AutoFactory>();
    }
    if (s_registro.find("Camion") == s_registro.end()) {
        s_registro["Camion"] = std::make_unique<CamionFactory>();
    }
}

/**
 * @brief Consulta si hay una fábrica registrada para el tipo dado.
 */
bool FactoriaVehiculos::tieneRegistro(const std::string& tipo)
{
    return s_registro.find(tipo) != s_registro.end();
}

} // namespace siger
