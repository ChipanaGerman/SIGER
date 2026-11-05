#pragma once
/**
 * @file Estrategias.h
 * @brief Functores de estrategia para asignación y filtrado de pedidos en SIGER.
 *
 * Implementa tres function-objects (structs con @c operator()) que encapsulan
 * algoritmos de asignación y filtrado desacoplados de las clases de dominio:
 *
 *  - @c AsignarPorPrioridad    : ordena un vector de pedidos por prioridad descendente.
 *  - @c FiltrarPorEstado       : filtra pedidos por su estado actual.
 *  - @c AsignarRepartidorOptimo: selecciona el repartidor libre con capacidad suficiente.
 *
 * Usando functores en lugar de funciones libres se facilita el almacenamiento
 * en std::function, la composición de estrategias y el testing unitario.
 *
 * @namespace siger
 */

#include "Pedido.h"
#include "Repartidor.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace siger {

// ─────────────────────────────────────────────────────────────────────────────
// Functor: AsignarPorPrioridad
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Ordena in-place un vector de pedidos por prioridad descendente.
 *
 * La prioridad de cada pedido se obtiene mediante @c Pedido::getPrioridad(),
 * que a su vez delega al paquete asociado. Los pedidos de mayor prioridad
 * quedan al frente del vector para ser procesados primero.
 *
 * Complejidad: O(n log n) donde n es el número de pedidos.
 *
 * Uso:
 * @code
 *   AsignarPorPrioridad estrategia;
 *   estrategia(pedidos);  // pedidos queda ordenado
 * @endcode
 */
struct AsignarPorPrioridad {
    /**
     * @brief Ordena @p pedidos por prioridad de mayor a menor (in-place).
     * @param pedidos Vector de shared_ptr<Pedido> a ordenar.
     */
    void operator()(std::vector<std::shared_ptr<Pedido>>& pedidos) const {
        std::sort(pedidos.begin(), pedidos.end(),
            [](const std::shared_ptr<Pedido>& a,
               const std::shared_ptr<Pedido>& b) {
                // Orden descendente: el de mayor prioridad va primero.
                return a->getPrioridad() > b->getPrioridad();
            }
        );
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Functor: FiltrarPorEstado
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Filtra pedidos conservando sólo los que coinciden con un @c EstadoPedido.
 *
 * El estado buscado se fija en construcción; el functor es entonces reutilizable
 * con distintos vectores de entrada sin necesidad de parámetros adicionales.
 *
 * Uso:
 * @code
 *   FiltrarPorEstado filtro(EstadoPedido::NUEVO);
 *   auto nuevos = filtro(todosLosPedidos);
 * @endcode
 */
struct FiltrarPorEstado {
    /**
     * @brief Construye el functor fijando el estado a buscar.
     * @param estado Estado de pedido por el que se desea filtrar.
     */
    explicit FiltrarPorEstado(EstadoPedido estado) : m_estado(estado) {}

    /**
     * @brief Devuelve un nuevo vector con los pedidos cuyo estado coincide.
     * @param pedidos Vector fuente (no se modifica).
     * @return Vector de shared_ptr<Pedido> con estado == m_estado.
     */
    std::vector<std::shared_ptr<Pedido>> operator()(
        const std::vector<std::shared_ptr<Pedido>>& pedidos) const
    {
        std::vector<std::shared_ptr<Pedido>> resultado;
        resultado.reserve(pedidos.size());

        std::copy_if(pedidos.cbegin(), pedidos.cend(),
                     std::back_inserter(resultado),
                     [this](const std::shared_ptr<Pedido>& p) {
                         return p->getEstado() == m_estado;
                     });

        return resultado;
    }

private:
    EstadoPedido m_estado; ///< Estado de pedido que se desea retener.
};

// ─────────────────────────────────────────────────────────────────────────────
// Functor: AsignarRepartidorOptimo
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Selecciona el repartidor óptimo para entregar un pedido dado.
 *
 * Criterios de selección (en orden de prioridad):
 *  1. El repartidor debe estar en estado @c EstadoRepartidor::LIBRE.
 *  2. Su vehículo debe poder cargar el peso del paquete del pedido
 *     (se consulta @c Vehiculo::puedeCargar()).
 *  3. Entre los candidatos elegibles, se elige el primero hallado
 *     (política FIFO sobre el vector recibido; la estrategia de orden
 *     puede combinarse con @c AsignarPorPrioridad externamente).
 *
 * Devuelve @c nullptr si no hay ningún repartidor disponible y con
 * capacidad suficiente para el pedido.
 *
 * Uso:
 * @code
 *   AsignarRepartidorOptimo estrategia;
 *   auto rep = estrategia(repartidores, pedido);
 *   if (rep) rep->asignarPedido(pedido);
 * @endcode
 */
struct AsignarRepartidorOptimo {
    /**
     * @brief Busca el repartidor libre y con capacidad para el pedido dado.
     *
     * @param repartidores Vector de shared_ptr<Repartidor> candidatos.
     * @param pedido       Pedido que se desea asignar.
     * @return shared_ptr<Repartidor> al repartidor seleccionado,
     *         o @c nullptr si ninguno es elegible.
     */
    std::shared_ptr<Repartidor> operator()(
        const std::vector<std::shared_ptr<Repartidor>>& repartidores,
        const std::shared_ptr<Pedido>&                  pedido) const
    {
        // Peso del paquete que debe poder cargar el vehículo.
        const double pesoKg = pedido->getPaquete()->getPesoKg();

        for (const auto& rep : repartidores) {
            // Criterio 1: debe estar LIBRE.
            if (rep->getEstado() != EstadoRepartidor::LIBRE) {
                continue;
            }

            // Criterio 2: su vehículo debe soportar el peso.
            auto vehiculo = rep->getVehiculo();
            if (vehiculo && vehiculo->puedeCargar(pesoKg)) {
                return rep; // Primer candidato válido (política FIFO).
            }
        }

        return nullptr; // Ningún repartidor disponible y capaz.
    }
};

} // namespace siger
