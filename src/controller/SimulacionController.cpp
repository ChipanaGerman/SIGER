/**
 * @file SimulacionController.cpp
 * @brief Implementación de SimulacionController.
 */

#include "SimulacionController.h"

namespace siger {

SimulacionController::SimulacionController(std::shared_ptr<SimuladorEntregas> simulador,
                                           std::shared_ptr<GeneradorEventos> generador,
                                           QObject* parent)
    : QObject(parent)
    , m_simulador(std::move(simulador))
    , m_generador(std::move(generador))
{}

void SimulacionController::onEvento(const DatoEvento& evento) {
    if (evento.tipo == EventoSiger::SIMULACION_INICIADA) {
        emit simulacionIniciada();
    } else if (evento.tipo == EventoSiger::SIMULACION_PAUSADA) {
        emit simulacionPausada();
    } else if (evento.tipo == EventoSiger::SIMULACION_DETENIDA) {
        emit simulacionDetenida();
    } else if (evento.tipo == EventoSiger::PEDIDO_COMPLETADO) {
        emit entregaCompletada(QString::fromStdString(evento.idEntidad), "");
    }

    if (m_simulador) {
        auto stats = m_simulador->getEstadisticas();
        emit estadoActualizado(stats.entregados, stats.fallidos, stats.enCamino);
    }
}

void SimulacionController::slotIniciar() {
    if (m_simulador) {
        m_simulador->iniciar();
    }
    if (m_generador) {
        m_generador->iniciar();
    }
}

void SimulacionController::slotPausar() {
    if (m_simulador) {
        m_simulador->pausar();
    }
}

void SimulacionController::slotReanudar() {
    if (m_simulador) {
        m_simulador->reanudar();
    }
}

void SimulacionController::slotDetener() {
    if (m_generador) {
        m_generador->detener();
    }
    if (m_simulador) {
        m_simulador->detener();
    }
}

void SimulacionController::slotSetVelocidad(int ms) {
    if (m_simulador) {
        m_simulador->setVelocidad(ms);
    }
}

} // namespace siger
