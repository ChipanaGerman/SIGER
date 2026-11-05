#pragma once
/**
 * @file   Cola.h
 * @brief  Cola genérica thread-safe para SIGER.
 *
 * Encapsula std::queue<T> protegida por std::mutex de modo que múltiples
 * hilos pueden insertar y extraer elementos de forma segura.
 *
 * @author SIGER Dev Team
 */

#include <mutex>
#include <optional>
#include <queue>
#include <string>

namespace siger {

/**
 * @class  Cola
 * @brief  Contenedor FIFO genérico thread-safe.
 *
 * @tparam T  Tipo de los elementos almacenados.
 */
template <typename T>
class Cola {
public:
    // ── Constructor / Destructor ──────────────────────────────────────────
    Cola()  = default;
    ~Cola() = default;

    // No copiable (el mutex no es copiable)
    Cola(const Cola&)            = delete;
    Cola& operator=(const Cola&) = delete;

    // Movible
    Cola(Cola&&)            = default;
    Cola& operator=(Cola&&) = default;

    // ── API pública ───────────────────────────────────────────────────────

    /**
     * @brief Inserta un elemento al final de la cola.
     * @param valor  Elemento a insertar.
     */
    void push(T valor) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(valor));
    }

    /**
     * @brief Extrae y devuelve el elemento del frente de la cola.
     *
     * Si la cola está vacía devuelve std::nullopt.
     *
     * @return std::optional<T> con el elemento, o std::nullopt si vacía.
     */
    [[nodiscard]] std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return std::nullopt;
        }
        T valor = std::move(m_queue.front());
        m_queue.pop();
        return valor;
    }

    /**
     * @brief Devuelve (sin extraer) el elemento del frente de la cola.
     *
     * Si la cola está vacía devuelve std::nullopt.
     *
     * @return std::optional<T> con el elemento, o std::nullopt si vacía.
     */
    [[nodiscard]] std::optional<T> peek() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return std::nullopt;
        }
        return m_queue.front();
    }

    /**
     * @brief Indica si la cola no contiene elementos.
     * @return true si vacía, false en caso contrario.
     */
    [[nodiscard]] bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    /**
     * @brief Devuelve el número de elementos en la cola.
     * @return Número de elementos.
     */
    [[nodiscard]] std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    /**
     * @brief Elimina todos los elementos de la cola.
     */
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) {
            m_queue.pop();
        }
    }

private:
    mutable std::mutex  m_mutex; ///< Mutex para proteger el acceso concurrente.
    std::queue<T>       m_queue; ///< Contenedor interno de elementos.
};

} // namespace siger
