#pragma once
/**
 * @file Observer.h
 * @brief Implementación del patrón de diseño Observer (GoF) para el sistema SIGER.
 *
 * Define:
 *  - @c EventoSiger  : enum class con todos los eventos del sistema.
 *  - @c DatoEvento   : struct payload tipado que acompaña cada notificación.
 *  - @c IObserver    : interfaz pura que debe implementar todo observador.
 *  - @c ISubject     : clase base que gestiona suscriptores y notificaciones.
 *
 * La lista de observadores usa @c std::weak_ptr para evitar ciclos de
 * referencia; los punteros expirados se purgan automáticamente en cada
 * notificación. Las operaciones sobre la lista son thread-safe.
 *
 * @namespace siger
 */

#include <algorithm>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace siger {

// ─────────────────────────────────────────────────────────────────────────────
// Enum: EventoSiger
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Lista exhaustiva de eventos que el sistema SIGER puede notificar.
 *
 * Cada valor identifica de manera inequívoca una transición de estado o
 * acción relevante dentro del ciclo de vida de pedidos y simulaciones.
 */
enum class EventoSiger {
    PEDIDO_NUEVO,               ///< Se registró un nuevo pedido.
    PEDIDO_ASIGNADO,            ///< El pedido fue asignado a un repartidor.
    PEDIDO_EN_CAMINO,           ///< El repartidor inició la entrega.
    PEDIDO_COMPLETADO,          ///< El pedido fue entregado exitosamente.
    PEDIDO_CANCELADO,           ///< El pedido fue cancelado.
    SIMULACION_INICIADA,        ///< La simulación arrancó.
    SIMULACION_PAUSADA,         ///< La simulación fue pausada.
    SIMULACION_DETENIDA,        ///< La simulación se detuvo completamente.
    ESTADISTICAS_ACTUALIZADAS   ///< Las estadísticas globales cambiaron.
};

// ─────────────────────────────────────────────────────────────────────────────
// Struct: DatoEvento
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Payload que acompaña a cada notificación de evento.
 *
 * Contiene el tipo de evento, el identificador de la entidad que lo originó
 * (pedido, repartidor, etc.) y una descripción legible para humanos o logs.
 */
struct DatoEvento {
    EventoSiger tipo;        ///< Tipo de evento ocurrido.
    std::string idEntidad;   ///< ID del pedido o repartidor que originó el evento.
    std::string descripcion; ///< Mensaje human-readable para logging/UI.
};

// ─────────────────────────────────────────────────────────────────────────────
// Interface: IObserver
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Interfaz pura que deben implementar todos los observadores de SIGER.
 *
 * Cualquier clase que desee recibir notificaciones de eventos del sistema debe
 * heredar de IObserver e implementar @c onEvento().
 */
class IObserver {
public:
    /// @brief Destructor virtual por defecto.
    virtual ~IObserver() = default;

    /**
     * @brief Callback invocado cuando ocurre un evento al que está suscrito.
     * @param dato Payload completo del evento con tipo, entidad y descripción.
     */
    virtual void onEvento(const DatoEvento& dato) = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// Class: ISubject
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Clase base para todo sujeto (publicador) en el patrón Observer.
 *
 * Gestiona la lista de observadores suscritos usando @c std::weak_ptr para
 * evitar ciclos de referencia. Limpia automáticamente los observadores que
 * han sido destruidos antes de cada notificación.
 *
 * @note Thread-safe: todas las operaciones sobre @c m_observers están
 *       protegidas por @c m_mutex.
 */
class ISubject {
public:
    /// @brief Destructor virtual por defecto.
    virtual ~ISubject() = default;

    /**
     * @brief Registra un observador para recibir notificaciones.
     *
     * Si el mismo observador ya está suscrito, la operación es idempotente
     * (no se duplica la entrada).
     *
     * @param obs Puntero compartido al observador a registrar.
     */
    void suscribir(std::shared_ptr<IObserver> obs) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Evitar duplicados comparando el puntero raw subyacente.
        for (const auto& weak : m_observers) {
            if (auto existing = weak.lock(); existing && existing.get() == obs.get()) {
                return; // Ya suscrito — operación idempotente.
            }
        }
        m_observers.emplace_back(obs);
    }

    /**
     * @brief Elimina un observador de la lista de suscriptores.
     *
     * Si el observador no estaba suscrito, la operación es segura (no-op).
     *
     * @param obs Puntero compartido al observador a eliminar.
     */
    void desuscribir(std::shared_ptr<IObserver> obs) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_observers.erase(
            std::remove_if(m_observers.begin(), m_observers.end(),
                [&obs](const std::weak_ptr<IObserver>& weak) {
                    auto locked = weak.lock();
                    // Eliminar si está expirado O si es el observador buscado.
                    return !locked || locked.get() == obs.get();
                }),
            m_observers.end()
        );
    }

protected:
    /**
     * @brief Notifica a todos los observadores activos con el evento dado.
     *
     * Antes de notificar, purga las entradas @c weak_ptr que hayan expirado.
     * Las notificaciones se realizan fuera del lock para evitar deadlocks en
     * caso de que un observador llame a @c suscribir / @c desuscribir
     * durante su @c onEvento.
     *
     * @param dato Payload del evento a propagar.
     */
    void notificarObservers(const DatoEvento& dato) {
        // Capturamos los shared_ptr bajo el lock.
        std::vector<std::shared_ptr<IObserver>> activos;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::vector<std::weak_ptr<IObserver>> vigentes;
            vigentes.reserve(m_observers.size());
            for (auto& weak : m_observers) {
                if (auto shared = weak.lock()) {
                    activos.push_back(shared);
                    vigentes.push_back(weak);
                }
                // Los weak_ptr expirados se descartan implícitamente.
            }
            m_observers = std::move(vigentes);
        }
        // Notificamos fuera del lock para evitar deadlocks.
        for (auto& obs : activos) {
            obs->onEvento(dato);
        }
    }

    /**
     * @brief Sobrecarga de conveniencia para notificar con tipo, id y descripción.
     */
    void notificarObservers(EventoSiger tipo,
                            const std::string& idEntidad = "",
                            const std::string& descripcion = "") {
        notificarObservers(DatoEvento{tipo, idEntidad, descripcion});
    }

    /**
     * @brief Sobrecarga de conveniencia que interpreta mensajes de texto.
     */
    void notificarObservers(const std::string& mensaje) {
        EventoSiger ev = EventoSiger::ESTADISTICAS_ACTUALIZADAS;
        std::string id = "";
        auto pos1 = mensaje.find(':');
        if (pos1 != std::string::npos) {
            auto pos2 = mensaje.find(':', pos1 + 1);
            if (pos2 != std::string::npos) {
                id = mensaje.substr(pos1 + 1, pos2 - pos1 - 1);
            } else {
                id = mensaje.substr(pos1 + 1);
            }
        }

        if (mensaje.find("PEDIDO_REGISTRADO") != std::string::npos || mensaje.find("PEDIDO_NUEVO") != std::string::npos) {
            ev = EventoSiger::PEDIDO_NUEVO;
        } else if (mensaje.find("PEDIDO_ASIGNADO") != std::string::npos) {
            ev = EventoSiger::PEDIDO_ASIGNADO;
        } else if (mensaje.find("EN_CAMINO") != std::string::npos) {
            ev = EventoSiger::PEDIDO_EN_CAMINO;
        } else if (mensaje.find("PEDIDO_COMPLETADO") != std::string::npos) {
            ev = EventoSiger::PEDIDO_COMPLETADO;
        } else if (mensaje.find("PEDIDO_CANCELADO") != std::string::npos || mensaje.find("PEDIDO_FALLIDO") != std::string::npos) {
            ev = EventoSiger::PEDIDO_CANCELADO;
        } else if (mensaje.find("SIMULACION_INICIADA") != std::string::npos) {
            ev = EventoSiger::SIMULACION_INICIADA;
        } else if (mensaje.find("SIMULACION_PAUSADA") != std::string::npos) {
            ev = EventoSiger::SIMULACION_PAUSADA;
        } else if (mensaje.find("SIMULACION_DETENIDA") != std::string::npos) {
            ev = EventoSiger::SIMULACION_DETENIDA;
        }
        notificarObservers(DatoEvento{ev, id, mensaje});
    }

private:
    /// @brief Lista de observadores suscritos (weak_ptr para evitar ciclos).
    std::vector<std::weak_ptr<IObserver>> m_observers;

    /// @brief Mutex para acceso thread-safe a @c m_observers.
    std::mutex m_mutex;
};

} // namespace siger
