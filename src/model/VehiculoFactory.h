#pragma once
/**
 * @file VehiculoFactory.h
 * @brief Patrón Factory Method (GoF) para la creación de vehículos en SIGER.
 *
 * Define la jerarquía de fábricas abstractas y concretas para instanciar
 * objetos Moto, Auto y Camion a través de una interfaz polimórfica.
 * También incluye el registro centralizado @c FactoriaVehiculos que permite
 * crear cualquier tipo de vehículo por nombre de cadena (Registry pattern).
 *
 * @namespace siger
 */

#include "Auto.h"
#include "Camion.h"
#include "Moto.h"
#include "Vehiculo.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace siger {

// ─────────────────────────────────────────────────────────────────────────────
// Abstract Factory: VehiculoFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Interfaz abstracta del Factory Method para vehículos.
 *
 * Cada subclase concreta es responsable de instanciar un único tipo de
 * vehículo. El polimorfismo permite al código cliente crear vehículos sin
 * conocer su clase concreta.
 */
class VehiculoFactory {
public:
    /// @brief Destructor virtual por defecto.
    virtual ~VehiculoFactory() = default;

    /**
     * @brief Factory Method — crea e inicializa un vehículo del tipo concreto.
     *
     * @param id          Identificador único del vehículo.
     * @param placa       Matrícula/placa del vehículo.
     * @param capacidadKg Capacidad máxima de carga en kilogramos.
     * @return shared_ptr al vehículo recién creado.
     */
    virtual std::shared_ptr<Vehiculo> crearVehiculo(
        const std::string& id,
        const std::string& placa,
        double             capacidadKg) const = 0;

    /**
     * @brief Devuelve el nombre del tipo de vehículo que produce esta fábrica.
     * @return Cadena con el tipo (p. ej. "Moto", "Auto", "Camion").
     */
    virtual std::string getTipoVehiculo() const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// Concrete Factory: MotoFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Fábrica concreta que produce instancias de @c Moto.
 */
class MotoFactory final : public VehiculoFactory {
public:
    /**
     * @brief Crea una nueva Moto.
     * @param id          Identificador único.
     * @param placa       Matrícula de la moto.
     * @param capacidadKg Capacidad de carga en kg.
     * @return shared_ptr<Vehiculo> apuntando a una instancia Moto.
     */
    std::shared_ptr<Vehiculo> crearVehiculo(
        const std::string& id,
        const std::string& placa,
        double             capacidadKg) const override;

    /**
     * @brief Devuelve el tipo de vehículo producido.
     * @return "Moto"
     */
    std::string getTipoVehiculo() const override;
};

// ─────────────────────────────────────────────────────────────────────────────
// Concrete Factory: AutoFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Fábrica concreta que produce instancias de @c Auto.
 */
class AutoFactory final : public VehiculoFactory {
public:
    /**
     * @brief Crea un nuevo Auto.
     * @param id          Identificador único.
     * @param placa       Matrícula del auto.
     * @param capacidadKg Capacidad de carga en kg.
     * @return shared_ptr<Vehiculo> apuntando a una instancia Auto.
     */
    std::shared_ptr<Vehiculo> crearVehiculo(
        const std::string& id,
        const std::string& placa,
        double             capacidadKg) const override;

    /**
     * @brief Devuelve el tipo de vehículo producido.
     * @return "Auto"
     */
    std::string getTipoVehiculo() const override;
};

// ─────────────────────────────────────────────────────────────────────────────
// Concrete Factory: CamionFactory
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Fábrica concreta que produce instancias de @c Camion.
 */
class CamionFactory final : public VehiculoFactory {
public:
    /**
     * @brief Crea un nuevo Camion.
     * @param id          Identificador único.
     * @param placa       Matrícula del camión.
     * @param capacidadKg Capacidad de carga en kg.
     * @return shared_ptr<Vehiculo> apuntando a una instancia Camion.
     */
    std::shared_ptr<Vehiculo> crearVehiculo(
        const std::string& id,
        const std::string& placa,
        double             capacidadKg) const override;

    /**
     * @brief Devuelve el tipo de vehículo producido.
     * @return "Camion"
     */
    std::string getTipoVehiculo() const override;
};

// ─────────────────────────────────────────────────────────────────────────────
// Registry: FactoriaVehiculos
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Registro centralizado de fábricas de vehículos (Registry Pattern).
 *
 * Permite registrar dinámicamente nuevas fábricas y crear vehículos por
 * nombre de tipo, desacoplando completamente al cliente de las clases
 * concretas. Los tres tipos estándar ("Moto", "Auto", "Camion") se
 * registran automáticamente en la primera llamada a @c registrarDefaults().
 *
 * @note Todos los métodos son estáticos; esta clase no se instancia.
 *
 * Ejemplo de uso:
 * @code
 *   FactoriaVehiculos::registrarDefaults();
 *   auto v = FactoriaVehiculos::crear("Moto", "V-001", "ABC-123", 15.0);
 * @endcode
 */
class FactoriaVehiculos {
public:
    /// @brief Constructor eliminado — clase puramente estática.
    FactoriaVehiculos() = delete;

    /**
     * @brief Registra una fábrica bajo un nombre de tipo.
     *
     * Si ya existe una entrada para @p tipo, se sobreescribe.
     *
     * @param tipo     Nombre del tipo de vehículo (p. ej. "Moto").
     * @param fabrica  unique_ptr a la fábrica concreta a registrar.
     */
    static void registrar(const std::string&           tipo,
                          std::unique_ptr<VehiculoFactory> fabrica);

    /**
     * @brief Crea un vehículo del tipo indicado usando la fábrica registrada.
     *
     * @param tipo        Nombre del tipo ("Moto", "Auto" o "Camion").
     * @param id          Identificador único del vehículo.
     * @param placa       Matrícula del vehículo.
     * @param capacidadKg Capacidad de carga en kg.
     * @return shared_ptr<Vehiculo> al vehículo creado.
     * @throws std::invalid_argument si @p tipo no está registrado.
     */
    static std::shared_ptr<Vehiculo> crear(
        const std::string& tipo,
        const std::string& id,
        const std::string& placa,
        double             capacidadKg);

    /**
     * @brief Registra las tres fábricas estándar del sistema.
     *
     * Idempotente: si ya fueron registradas no duplica entradas.
     * Debe llamarse una vez al inicio de la aplicación o antes de la primera
     * llamada a @c crear().
     */
    static void registrarDefaults();

    /**
     * @brief Indica si un tipo de vehículo está disponible en el registro.
     * @param tipo Nombre del tipo a consultar.
     * @return @c true si existe una fábrica registrada para ese tipo.
     */
    static bool tieneRegistro(const std::string& tipo);

private:
    /// @brief Mapa de fábricas indexadas por nombre de tipo de vehículo.
    static std::map<std::string, std::unique_ptr<VehiculoFactory>> s_registro;
};

} // namespace siger
