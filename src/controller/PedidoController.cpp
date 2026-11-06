/**
 * @file PedidoController.cpp
 * @brief Implementación de PedidoController.
 */

#include "PedidoController.h"
#include "PaqueteEstandar.h"
#include "PaqueteFragil.h"
#include "Pedido.h"

#include <sstream>

namespace siger {

PedidoController::PedidoController(std::shared_ptr<GestorPedidos> gestor, QObject* parent)
    : QObject(parent)
    , m_gestor(std::move(gestor))
{}

void PedidoController::onEvento(const DatoEvento& evento) {
    if (evento.tipo == EventoSiger::PEDIDO_NUEVO) {
        if (m_gestor) {
            auto optPedido = m_gestor->obtenerPedido(evento.idEntidad);
            if (optPedido.has_value()) {
                auto ped = optPedido.value();
                emit pedidoAgregado(
                    QString::fromStdString(ped->getId()),
                    QString::fromStdString(ped->getPaquete() ? ped->getPaquete()->getTipo() : "Estándar"),
                    QString::fromStdString(ped->getEstadoStr()),
                    ped->getPaquete() ? ped->getPaquete()->getPesoKg() : 1.0,
                    QString::fromStdString(ped->getOrigen()),
                    QString::fromStdString(ped->getDestino())
                );
            }
            auto stats = m_gestor->getEstadisticas();
            emit estadisticasActualizadas(stats.total,
                                          stats.pendientes,
                                          stats.completados,
                                          stats.cancelados);
        }
    } else if (evento.tipo == EventoSiger::PEDIDO_EN_CAMINO) {
        emit pedidoActualizado(QString::fromStdString(evento.idEntidad), "EN CAMINO");
        if (m_gestor) {
            auto optPedido = m_gestor->obtenerPedido(evento.idEntidad);
            if (optPedido.has_value()) {
                auto ped = optPedido.value();
                emit pedidoEnCamino(
                    QString::fromStdString(ped->getId()),
                    QString::fromStdString(ped->getOrigen()),
                    QString::fromStdString(ped->getDestino()),
                    QString::fromStdString(ped->getRepartidorId())
                );
            }
            auto stats = m_gestor->getEstadisticas();
            emit estadisticasActualizadas(stats.total,
                                          stats.pendientes,
                                          stats.completados,
                                          stats.cancelados);
        }
    } else if (evento.tipo == EventoSiger::PEDIDO_COMPLETADO) {
        emit pedidoActualizado(QString::fromStdString(evento.idEntidad), "COMPLETADO");
        if (m_gestor) {
            // Forzamos el conteo ANTES de leer estadísticas, por si GestorPedidos
            // aún no procesó su onEvento (orden de observers no garantizado).
            m_gestor->marcarCompletado(evento.idEntidad);
            auto stats = m_gestor->getEstadisticas();
            emit estadisticasActualizadas(stats.total,
                                          stats.pendientes,
                                          stats.completados,
                                          stats.cancelados);
        }
    } else if (evento.tipo == EventoSiger::PEDIDO_CANCELADO) {
        emit pedidoActualizado(QString::fromStdString(evento.idEntidad), "CANCELADO");
        if (m_gestor) {
            auto stats = m_gestor->getEstadisticas();
            emit estadisticasActualizadas(stats.total,
                                          stats.pendientes,
                                          stats.completados,
                                          stats.cancelados);
        }
    }
}

void PedidoController::slotAgregarPedido(const QString& tipo,
                                         const QString& descripcion,
                                         double peso,
                                         const QString& origen,
                                         const QString& destino,
                                         double distancia,
                                         const QString& /*tipoVehiculo*/) {
    if (!m_gestor) return;

    std::ostringstream oss;
    oss << "PED-" << ++m_contadorId;
    std::string id = oss.str();

    std::shared_ptr<Paquete> paquete;
    if (tipo.contains("Frágil", Qt::CaseInsensitive) || tipo.contains("Fragil", Qt::CaseInsensitive)) {
        paquete = std::make_shared<PaqueteFragil>(
            "PAQ-" + id,
            descripcion.toStdString(),
            peso,
            destino.toStdString(),
            1200.0 // valor asegurado base
        );
    } else {
        paquete = std::make_shared<PaqueteEstandar>(
            "PAQ-" + id,
            descripcion.toStdString(),
            peso,
            destino.toStdString(),
            2 // dias entrega base
        );
    }

    auto pedido = std::make_shared<Pedido>(
        id,
        paquete,
        origen.toStdString(),
        destino.toStdString(),
        distancia
    );

    // Al registrarlo, el gestor notifica PEDIDO_NUEVO a onEvento,
    // el cual se encarga de emitir pedidoAgregado y estadisticasActualizadas.
    m_gestor->registrarPedido(pedido);
}

void PedidoController::slotCancelarPedido(const QString& id) {
    if (!m_gestor) return;
    m_gestor->cancelarPedido(id.toStdString());
}

void PedidoController::slotRefrescarTodos() {
    if (!m_gestor) return;

    auto pedidos = m_gestor->getPedidosActivos();
    for (const auto& ped : pedidos) {
        if (!ped) continue;
        emit pedidoAgregado(
            QString::fromStdString(ped->getId()),
            QString::fromStdString(ped->getPaquete() ? ped->getPaquete()->getTipo() : "N/A"),
            QString::fromStdString(ped->getEstadoStr()),
            ped->getPaquete() ? ped->getPaquete()->getPesoKg() : 0.0,
            QString::fromStdString(ped->getOrigen()),
            QString::fromStdString(ped->getDestino())
        );
    }

    auto stats = m_gestor->getEstadisticas();
    emit estadisticasActualizadas(stats.total,
                                  stats.pendientes,
                                  stats.completados,
                                  stats.cancelados);
}

} // namespace siger
