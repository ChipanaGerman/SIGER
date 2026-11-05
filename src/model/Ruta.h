#pragma once
/**
 * @file   Ruta.h
 * @brief  Ruta de entrega genérica para SIGER.
 *
 * Almacena una secuencia de puntos coordenados (lat/lon o x/y) con sus
 * etiquetas, calcula la distancia total euclídea y ofrece un operador de
 * impresión para resumen rápido.
 *
 * @author SIGER Dev Team
 */

#include <cmath>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace siger {

/**
 * @class  Ruta
 * @brief  Secuencia ordenada de puntos geográficos o cartesianos que forma
 *         una ruta de entrega.
 *
 * @tparam CoordType  Tipo numérico de las coordenadas (por defecto double).
 */
template <typename CoordType = double>
class Ruta {
public:
    // ── Tipos ─────────────────────────────────────────────────────────────
    using Punto = std::pair<CoordType, CoordType>;

    // ── Constructor / Destructor ──────────────────────────────────────────
    Ruta()  = default;
    ~Ruta() = default;

    // ── API pública ───────────────────────────────────────────────────────

    /**
     * @brief Agrega un punto al final de la ruta.
     *
     * @param x        Primera componente de la coordenada (latitud o X).
     * @param y        Segunda componente de la coordenada (longitud o Y).
     * @param etiqueta Nombre descriptivo del lugar (p.ej. "Almacén Central").
     */
    void agregarPunto(CoordType x, CoordType y, std::string etiqueta = "") {
        m_puntos.emplace_back(x, y);
        m_etiquetas.push_back(std::move(etiqueta));
    }

    /**
     * @brief Calcula la distancia total euclídea de la ruta.
     *
     * Suma las distancias entre puntos consecutivos:
     *   d = sqrt((x2-x1)^2 + (y2-y1)^2)
     *
     * @return Distancia acumulada en las mismas unidades que las coordenadas.
     *         Devuelve CoordType(0) si hay menos de dos puntos.
     */
    [[nodiscard]] CoordType calcularDistanciaTotal() const {
        if (m_puntos.size() < 2) {
            return CoordType(0);
        }
        CoordType total = CoordType(0);
        for (std::size_t i = 0; i + 1 < m_puntos.size(); ++i) {
            const auto dx = m_puntos[i + 1].first  - m_puntos[i].first;
            const auto dy = m_puntos[i + 1].second - m_puntos[i].second;
            total += static_cast<CoordType>(std::sqrt(dx * dx + dy * dy));
        }
        return total;
    }

    /**
     * @brief Accede al vector de puntos de la ruta (solo lectura).
     * @return Referencia constante al vector de pares de coordenadas.
     */
    [[nodiscard]] const std::vector<Punto>& getPuntos() const noexcept {
        return m_puntos;
    }

    /**
     * @brief Accede al vector de etiquetas de cada punto (solo lectura).
     * @return Referencia constante al vector de etiquetas.
     */
    [[nodiscard]] const std::vector<std::string>& getEtiquetas() const noexcept {
        return m_etiquetas;
    }

    /**
     * @brief Devuelve el número de puntos en la ruta.
     * @return Número de puntos.
     */
    [[nodiscard]] std::size_t size() const noexcept {
        return m_puntos.size();
    }

    /**
     * @brief Elimina todos los puntos y etiquetas de la ruta.
     */
    void limpiar() {
        m_puntos.clear();
        m_etiquetas.clear();
    }

    /**
     * @brief Escribe un resumen de la ruta en un flujo de salida.
     *
     * Formato:
     * @code
     * Ruta [N puntos | distancia total: D]
     *   0: Etiqueta0 (x0, y0)
     *   1: Etiqueta1 (x1, y1)
     *   ...
     * @endcode
     *
     * @param os    Flujo de salida destino.
     * @param ruta  Ruta a imprimir.
     * @return Referencia al flujo de salida.
     */
    friend std::ostream& operator<<(std::ostream& os, const Ruta<CoordType>& ruta) {
        os << "Ruta [" << ruta.size()
           << " puntos | distancia total: "
           << ruta.calcularDistanciaTotal() << "]\n";

        for (std::size_t i = 0; i < ruta.m_puntos.size(); ++i) {
            os << "  " << i << ": "
               << (ruta.m_etiquetas[i].empty() ? "(sin etiqueta)" : ruta.m_etiquetas[i])
               << " (" << ruta.m_puntos[i].first
               << ", " << ruta.m_puntos[i].second << ")\n";
        }
        return os;
    }

private:
    std::vector<Punto>       m_puntos;    ///< Coordenadas de cada punto de la ruta.
    std::vector<std::string> m_etiquetas; ///< Nombre descriptivo de cada punto.
};

} // namespace siger
