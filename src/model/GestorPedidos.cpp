/**
 * @file GestorPedidos.cpp
 * @brief Implementación de GestorPedidos.
 */

#include "GestorPedidos.h"
#include <algorithm>

namespace siger {

GestorPedidos::GestorPedidos(std::shared_ptr<Cola<std::shared_ptr<Pedido>>> cola)
    : m_colaEspera(cola ? std::move(cola) : std::make_shared<Cola<std::shared_ptr<Pedido>>>())
{}

void GestorPedidos::onEvento(const DatoEvento& evento) {
    if (evento.tipo == EventoSiger::PEDIDO_COMPLETADO) {
        marcarCompletado(evento.idEntidad);
    } else if (evento.tipo == EventoSiger::PEDIDO_CANCELADO) {
        auto opt = m_repositorioPedidos.obtener(evento.idEntidad);
        if (opt.has_value() && opt.value()->getEstado() != EstadoPedido::FALLIDO) {
            opt.value()->setEstado(EstadoPedido::FALLIDO);
            ++m_pedidosCancelados;
        }
    }
}

bool GestorPedidos::registrarPedido(std::shared_ptr<Pedido> pedido) {
    if (!pedido) return false;

    const std::string& id = pedido->getId();
    if (!m_repositorioPedidos.agregar(id, pedido)) {
        return false;
    }

    if (m_colaEspera) {
        m_colaEspera->push(pedido);
    }

    notificarObservers(EventoSiger::PEDIDO_NUEVO, id, "Nuevo pedido registrado: " + id);
    return true;
}

bool GestorPedidos::cancelarPedido(const std::string& id) {
    auto optPedido = m_repositorioPedidos.obtener(id);
    if (!optPedido.has_value()) {
        return false;
    }

    auto& pedido = optPedido.value();
    if (pedido->getEstado() == EstadoPedido::COMPLETADO ||
        pedido->getEstado() == EstadoPedido::FALLIDO) {
        return false;
    }

    pedido->setEstado(EstadoPedido::FALLIDO);
    if (pedido->getPaquete()) {
        pedido->getPaquete()->setEstado(EstadoPaquete::CANCELADO);
    }
    ++m_pedidosCancelados;

    notificarObservers(EventoSiger::PEDIDO_CANCELADO, id, "Pedido cancelado: " + id);
    return true;
}

void GestorPedidos::marcarCompletado(const std::string& id) {
    auto opt = m_repositorioPedidos.obtener(id);
    if (!opt.has_value()) return;

    auto& pedido = opt.value();
    if (pedido->getEstado() != EstadoPedido::COMPLETADO) {
        pedido->setEstado(EstadoPedido::COMPLETADO);
        if (pedido->getPaquete()) {
            pedido->getPaquete()->setEstado(EstadoPaquete::ENTREGADO);
        }
        ++m_pedidosCompletados;
    }
}

std::optional<std::shared_ptr<Pedido>> GestorPedidos::obtenerSiguiente() {
    if (!m_colaEspera) return std::nullopt;

    auto optPedido = m_colaEspera->pop();
    if (!optPedido.has_value()) {
        return std::nullopt;
    }

    auto pedido = optPedido.value();
    while (pedido && pedido->getEstado() == EstadoPedido::FALLIDO) {
        optPedido = m_colaEspera->pop();
        if (!optPedido.has_value()) {
            return std::nullopt;
        }
        pedido = optPedido.value();
    }

    if (pedido) {
        notificarObservers(EventoSiger::PEDIDO_ASIGNADO, pedido->getId(), "Pedido despachado de cola: " + pedido->getId());
    }
    return optPedido;
}

std::optional<std::shared_ptr<Pedido>> GestorPedidos::obtenerPedido(const std::string& id) const {
    return m_repositorioPedidos.obtener(id);
}

std::vector<std::shared_ptr<Pedido>> GestorPedidos::getPedidosActivos() const {
    return m_repositorioPedidos.buscar([](const std::shared_ptr<Pedido>& p) {
        return p &&
               p->getEstado() != EstadoPedido::COMPLETADO &&
               p->getEstado() != EstadoPedido::FALLIDO;
    });
}

EstadisticasPedidos GestorPedidos::getEstadisticas() const {
    EstadisticasPedidos stats;
    stats.total        = static_cast<int>(m_repositorioPedidos.cantidad());
    stats.completados  = m_pedidosCompletados.load();
    stats.cancelados   = m_pedidosCancelados.load();
    int pendientes = stats.total - (stats.completados + stats.cancelados);
    stats.pendientes   = std::max(0, pendientes);
    return stats;
}

} // namespace siger
