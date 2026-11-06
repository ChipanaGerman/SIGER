#pragma once
/**
 * @file Pedido.h
 * @brief Representa un pedido de entrega: vincula paquete, origen, destino y estado.
 */
#include "Paquete.h"
#include <string>
#include <memory>
#include <chrono>

namespace siger {

enum class EstadoPedido {
    NUEVO,
    ASIGNADO,
    EN_CAMINO,
    COMPLETADO,
    FALLIDO
};

class Pedido {
public:
    Pedido(const std::string& id,
           std::shared_ptr<Paquete> paquete,
           const std::string& origen,
           const std::string& destino,
           double distanciaKm,
           const std::string& tipoVehiculo = "");

    // ── Getters ──────────────────────────────────────────────────────
    const std::string&            getId()          const { return m_id; }
    std::shared_ptr<Paquete>      getPaquete()     const { return m_paquete; }
    const std::string&            getOrigen()      const { return m_origen; }
    const std::string&            getDestino()     const { return m_destino; }
    double                        getDistanciaKm() const { return m_distanciaKm; }
    EstadoPedido                  getEstado()      const { return m_estado; }
    const std::string&            getRepartidorId() const { return m_repartidorId; }
    std::string                   getTipoVehiculoRequerido() const;

    // ── Verificación de compatibilidad ───────────────────────────────
    bool esCompatibleCon(const std::string& tipoVehiculo, double capacidadKg) const;

    // ── Setters ──────────────────────────────────────────────────────
    void setEstado(EstadoPedido estado)         { m_estado = estado; }
    void setRepartidorId(const std::string& id) { m_repartidorId = id; }

    std::string getEstadoStr() const;
    int getPrioridad() const;  // delega al paquete

private:
    std::string              m_id;
    std::shared_ptr<Paquete> m_paquete;
    std::string              m_origen;
    std::string              m_destino;
    double                   m_distanciaKm;
    EstadoPedido             m_estado { EstadoPedido::NUEVO };
    std::string              m_repartidorId;
    std::string              m_tipoVehiculoRequerido;
};

} // namespace siger
