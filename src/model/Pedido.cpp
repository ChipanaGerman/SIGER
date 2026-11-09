/**
 * @file Pedido.cpp
 */
#include "Pedido.h"
#include "IEstrategiaTarifa.h"

namespace siger {

Pedido::Pedido(const std::string& id,
               std::shared_ptr<Paquete> paquete,
               const std::string& origen,
               const std::string& destino,
               double distanciaKm,
               const std::string& tipoVehiculo)
    : m_id(id)
    , m_paquete(std::move(paquete))
    , m_origen(origen)
    , m_destino(destino)
    , m_distanciaKm(distanciaKm)
    , m_tipoVehiculoRequerido(tipoVehiculo)
{
    // Cálculo inicial de tarifa según estrategia adecuada
    double peso = m_paquete ? m_paquete->getPesoKg() : 1.0;
    bool esFragil = m_paquete && m_paquete->requiereManejoEspecial();
    if (peso > 100.0) {
        m_costoEnvio = TarifaPesadaStrategy().calcularTarifa(m_distanciaKm, peso, esFragil);
    } else if (esFragil) {
        m_costoEnvio = TarifaFragilStrategy().calcularTarifa(m_distanciaKm, peso, esFragil);
    } else {
        m_costoEnvio = TarifaEstandarStrategy().calcularTarifa(m_distanciaKm, peso, esFragil);
    }
}

std::string Pedido::getTipoVehiculoRequerido() const {
    if (!m_tipoVehiculoRequerido.empty()) {
        return m_tipoVehiculoRequerido;
    }
    double peso = m_paquete ? m_paquete->getPesoKg() : 1.0;
    if (peso <= 15.0) return "Moto";
    if (peso <= 100.0) return "Auto";
    return "Camion";
}

bool Pedido::esCompatibleCon(const std::string& tipoVehiculo, double capacidadKg) const {
    double peso = m_paquete ? m_paquete->getPesoKg() : 0.0;
    if (peso > capacidadKg) {
        return false;
    }
    // Si se exigió un vehículo específico (desde GUI), debe coincidir exactamente
    if (!m_tipoVehiculoRequerido.empty()) {
        return m_tipoVehiculoRequerido == tipoVehiculo;
    }
    // Si no se exigió explícitamente (tests unitarios), cualquier vehículo con capacidad basta
    return true;
}

std::string Pedido::getEstadoStr() const {
    switch (m_estado) {
        case EstadoPedido::NUEVO:       return "NUEVO";
        case EstadoPedido::ASIGNADO:    return "ASIGNADO";
        case EstadoPedido::EN_CAMINO:   return "EN CAMINO";
        case EstadoPedido::COMPLETADO:  return "COMPLETADO";
        case EstadoPedido::FALLIDO:     return "FALLIDO";
        default:                        return "DESCONOCIDO";
    }
}

int Pedido::getPrioridad() const {
    return m_paquete ? m_paquete->calcularPrioridad() : 0;
}

} // namespace siger
