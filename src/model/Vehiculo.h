#pragma once
/**
 * @file Vehiculo.h
 * @brief Clase base abstracta para todos los vehículos de reparto.
 *
 * Demuestra: clase abstracta, funciones virtuales puras, encapsulamiento.
 */
#include <string>
#include <memory>

namespace siger {

/**
 * @brief Estado operativo de un vehículo.
 */
enum class EstadoVehiculo {
    DISPONIBLE,
    EN_RUTA,
    EN_MANTENIMIENTO,
    FUERA_DE_SERVICIO
};

/**
 * @brief Clase abstracta base para vehículos de reparto.
 *
 * Define la interfaz polimórfica que todas las subclases deben implementar.
 */
class Vehiculo {
public:
    Vehiculo(const std::string& id,
             const std::string& placa,
             double capacidadKg,
             double velocidadKmh);

    virtual ~Vehiculo() = default;

    // ── Interfaz polimórfica (funciones virtuales puras) ─────────────
    /**
     * @brief Calcula el costo de entrega basado en distancia y peso.
     * @param distanciaKm Distancia del recorrido en km.
     * @param pesoKg      Peso del paquete en kg.
     * @return Costo calculado en soles.
     */
    virtual double calcularCosto(double distanciaKm, double pesoKg) const = 0;

    /**
     * @brief Devuelve el tipo de vehículo como cadena.
     */
    virtual std::string getTipo() const = 0;

    /**
     * @brief Calcula el tiempo estimado de entrega en minutos.
     */
    virtual double calcularTiempoEntrega(double distanciaKm) const = 0;

    // ── Funciones virtuales con implementación por defecto ───────────
    virtual std::string getDescripcion() const;
    virtual bool puedeCargar(double pesoKg) const;

    // ── Getters (encapsulamiento) ────────────────────────────────────
    const std::string& getId()       const { return m_id; }
    const std::string& getPlaca()    const { return m_placa; }
    double  getCapacidadKg()         const { return m_capacidadKg; }
    double  getVelocidadKmh()        const { return m_velocidadKmh; }
    EstadoVehiculo getEstado()       const { return m_estado; }

    // ── Setters ──────────────────────────────────────────────────────
    void setEstado(EstadoVehiculo estado) { m_estado = estado; }
    void setVelocidadKmh(double v)        { m_velocidadKmh = v; }

protected:
    std::string    m_id;
    std::string    m_placa;
    double         m_capacidadKg;
    double         m_velocidadKmh;
    EstadoVehiculo m_estado { EstadoVehiculo::DISPONIBLE };
};

} // namespace siger
