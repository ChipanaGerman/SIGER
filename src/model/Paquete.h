#pragma once
/**
 * @file Paquete.h
 * @brief Clase base abstracta para paquetes a entregar.
 *
 * Demuestra: encapsulamiento (atributos privados + getters/setters),
 *            clase abstracta con función virtual pura calcularPrioridad().
 */
#include <string>
#include <chrono>

namespace siger {

enum class EstadoPaquete {
    PENDIENTE,
    ASIGNADO,
    EN_TRANSITO,
    ENTREGADO,
    CANCELADO
};

/**
 * @brief Clase abstracta base para paquetes.
 */
class Paquete {
public:
    Paquete(const std::string& id,
            const std::string& descripcion,
            double pesoKg,
            const std::string& direccionDestino);

    virtual ~Paquete() = default;

    // ── Interfaz polimórfica ─────────────────────────────────────────
    /**
     * @brief Calcula la prioridad del paquete (mayor = más urgente).
     * @return Valor de prioridad entre 1 y 10.
     */
    virtual int calcularPrioridad() const = 0;

    /**
     * @brief Devuelve el tipo de paquete como texto.
     */
    virtual std::string getTipo() const = 0;

    /**
     * @brief Indica si el paquete requiere manejo especial.
     */
    virtual bool requiereManejoEspecial() const = 0;

    // ── Getters ──────────────────────────────────────────────────────
    const std::string& getId()               const { return m_id; }
    const std::string& getDescripcion()      const { return m_descripcion; }
    double             getPesoKg()           const { return m_pesoKg; }
    const std::string& getDireccionDestino() const { return m_direccionDestino; }
    EstadoPaquete      getEstado()           const { return m_estado; }

    // ── Setters ──────────────────────────────────────────────────────
    void setEstado(EstadoPaquete estado) { m_estado = estado; }
    void setDireccionDestino(const std::string& dir) { m_direccionDestino = dir; }

    std::string getEstadoStr() const;

protected:
    std::string   m_id;
    std::string   m_descripcion;
    double        m_pesoKg;
    std::string   m_direccionDestino;
    EstadoPaquete m_estado { EstadoPaquete::PENDIENTE };
};

} // namespace siger
