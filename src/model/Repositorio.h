#pragma once
/**
 * @file   Repositorio.h
 * @brief  Repositorio genérico de entidades para SIGER.
 *
 * Almacena entidades usando std::unordered_map y ofrece acceso concurrente
 * seguro mediante std::shared_mutex (lecturas simultáneas, escritura exclusiva).
 *
 * @author SIGER Dev Team
 */

#include <functional>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace siger {

/**
 * @class  Repositorio
 * @brief  Almacén clave-valor genérico con acceso thread-safe.
 *
 * @tparam T    Tipo de la entidad almacenada.
 * @tparam Key  Tipo de la clave (por defecto std::string).
 */
template <typename T, typename Key = std::string>
class Repositorio {
public:
    // ── Tipos ─────────────────────────────────────────────────────────────
    using EntidadPtr = std::shared_ptr<T>;
    using Mapa       = std::unordered_map<Key, EntidadPtr>;

    // ── Constructor / Destructor ──────────────────────────────────────────
    Repositorio()  = default;
    ~Repositorio() = default;

    // No copiable (shared_mutex no es copiable)
    Repositorio(const Repositorio&)            = delete;
    Repositorio& operator=(const Repositorio&) = delete;

    // Movible
    Repositorio(Repositorio&&)            = default;
    Repositorio& operator=(Repositorio&&) = default;

    // ── API pública ───────────────────────────────────────────────────────

    /**
     * @brief Agrega una entidad al repositorio.
     *
     * No sobreescribe si la clave ya existe.
     *
     * @param clave   Identificador único.
     * @param entidad Puntero compartido a la entidad.
     * @return true si se insertó; false si la clave ya existía.
     */
    bool agregar(const Key& clave, EntidadPtr entidad) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        auto [it, inserted] = m_mapa.emplace(clave, std::move(entidad));
        return inserted;
    }

    /**
     * @brief Obtiene la entidad asociada a la clave.
     * @param clave  Identificador de la entidad.
     * @return std::optional<EntidadPtr> con el puntero, o std::nullopt si no existe.
     */
    [[nodiscard]] std::optional<EntidadPtr> obtener(const Key& clave) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        auto it = m_mapa.find(clave);
        if (it == m_mapa.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    /**
     * @brief Elimina la entidad con la clave dada.
     * @param clave  Identificador de la entidad a eliminar.
     * @return true si se eliminó; false si la clave no existía.
     */
    bool eliminar(const Key& clave) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        return m_mapa.erase(clave) > 0;
    }

    /**
     * @brief Comprueba si una clave está registrada en el repositorio.
     * @param clave  Identificador a verificar.
     * @return true si existe.
     */
    [[nodiscard]] bool existe(const Key& clave) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_mapa.count(clave) > 0;
    }

    /**
     * @brief Devuelve todos los punteros almacenados en el repositorio.
     * @return Vector de punteros compartidos.
     */
    [[nodiscard]] std::vector<EntidadPtr> obtenerTodos() const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        std::vector<EntidadPtr> resultado;
        resultado.reserve(m_mapa.size());
        for (const auto& [clave, entidad] : m_mapa) {
            resultado.push_back(entidad);
        }
        return resultado;
    }

    /**
     * @brief Busca entidades que satisfagan un predicado.
     *
     * @tparam Predicado  Callable con firma bool(const EntidadPtr&).
     * @param  pred       Función de filtrado.
     * @return Vector con las entidades que cumplieron el predicado.
     */
    template <typename Predicado>
    [[nodiscard]] std::vector<EntidadPtr> buscar(Predicado pred) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        std::vector<EntidadPtr> resultado;
        for (const auto& [clave, entidad] : m_mapa) {
            if (pred(entidad)) {
                resultado.push_back(entidad);
            }
        }
        return resultado;
    }

    /**
     * @brief Devuelve el número de entidades almacenadas.
     * @return Cantidad de entidades.
     */
    [[nodiscard]] std::size_t cantidad() const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_mapa.size();
    }

    /**
     * @brief Elimina todas las entidades del repositorio.
     */
    void limpiar() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_mapa.clear();
    }

private:
    mutable std::shared_mutex m_mutex; ///< Mutex lector/escritor.
    Mapa                      m_mapa;  ///< Contenedor interno.
};

} // namespace siger
