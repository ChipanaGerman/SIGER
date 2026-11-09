#pragma once
/**
 * @file ServicioRutas.h
 * @brief Servicio de enrutamiento vehicular urbano para SIGER con integración a API OSRM y caché local.
 *
 * Aplica el patrón de diseño Singleton (GoF) para garantizar un punto de acceso global
 * y unificado a la base de datos de geometrías viales reales de Arequipa.
 * Provee polilíneas detalladas calle por calle, distancias métricas de carretera
 * y tiempos estimados de recorrido para todos los vehículos.
 *
 * @author L. Baca <lbacac@unsa.edu.pe>
 * @author SIGER Dev Team
 */

#include <QString>
#include <QPointF>
#include <QVector>
#include <QMap>

namespace siger {

/**
 * @struct RutaUrbana
 * @brief Estructura que almacena la geometría y métricas de una ruta vial por calles.
 */
struct RutaUrbana {
    double distanciaMetros{0.0};
    double duracionSegundos{0.0};
    QVector<QPointF> waypoints;
    bool encontrada{false};
};

/**
 * @class ServicioRutas
 * @brief Singleton que gestiona la topología vial y enrutamiento vehicular urbano de Arequipa.
 */
class ServicioRutas {
public:
    /**
     * @brief Obtiene la instancia única global del servicio (Meyer's Singleton thread-safe).
     */
    static ServicioRutas& instancia();

    /**
     * @brief Carga las rutas vehiculares precalculadas desde recursos Qt (.qrc) o disco.
     * @param rutaArchivo Ruta opcional al archivo JSON.
     * @return true si se cargaron exitosamente.
     */
    bool inicializar(const QString& rutaArchivo = "");

    /**
     * @brief Obtiene la ruta vial detallada (waypoints calle por calle) entre origen y destino.
     */
    RutaUrbana obtenerRuta(const QString& origen, const QString& destino) const;

    /**
     * @brief Retorna la distancia de conducción real en kilómetros entre origen y destino.
     */
    double calcularDistanciaKm(const QString& origen, const QString& destino) const;

    /**
     * @brief Retorna la duración estimada de viaje en minutos.
     */
    double calcularDuracionMin(const QString& origen, const QString& destino) const;

    /**
     * @brief Retorna la cantidad de rutas viales cacheadas en memoria.
     */
    int cantidadRutas() const { return m_rutas.size(); }

    /**
     * @brief Verifica si una ruta específica está cargada en el repositorio.
     */
    bool tieneRuta(const QString& origen, const QString& destino) const;

private:
    ServicioRutas();
    ~ServicioRutas() = default;
    ServicioRutas(const ServicioRutas&) = delete;
    ServicioRutas& operator=(const ServicioRutas&) = delete;

    QString generarClave(const QString& origen, const QString& destino) const;
    QString normalizarNombre(const QString& nombre) const;

    QMap<QString, RutaUrbana> m_rutas;
    bool m_inicializado{false};
};

} // namespace siger
