/**
 * @file ServicioRutas.cpp
 * @brief Implementación del servicio de enrutamiento vial urbano de Arequipa.
 */

#include "ServicioRutas.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDebug>
#include <cmath>

namespace siger {

ServicioRutas& ServicioRutas::instancia() {
    static ServicioRutas s_instancia;
    return s_instancia;
}

ServicioRutas::ServicioRutas() {
    inicializar();
}

QString ServicioRutas::normalizarNombre(const QString& nombre) const {
    QString limpio = nombre.trimmed();
    // Normalización de caracteres especiales y acentos para compatibilidad
    limpio.replace("é", "e").replace("É", "E");
    limpio.replace("á", "a").replace("Á", "A");
    limpio.replace("í", "i").replace("Í", "I");
    limpio.replace("ó", "o").replace("Ó", "O");
    limpio.replace("ú", "u").replace("Ú", "U");
    return limpio;
}

QString ServicioRutas::generarClave(const QString& origen, const QString& destino) const {
    return normalizarNombre(origen) + "|" + normalizarNombre(destino);
}

bool ServicioRutas::inicializar(const QString& rutaArchivo) {
    if (m_inicializado && !m_rutas.isEmpty() && rutaArchivo.isEmpty()) {
        return true;
    }

    QStringList posiblesRutas;
    if (!rutaArchivo.isEmpty()) {
        posiblesRutas.append(rutaArchivo);
    }
    posiblesRutas << ":/resources/rutas_arequipa.json"
                  << "resources/rutas_arequipa.json"
                  << "../resources/rutas_arequipa.json"
                  << "../../resources/rutas_arequipa.json"
                  << QCoreApplication::applicationDirPath() + "/resources/rutas_arequipa.json"
                  << QCoreApplication::applicationDirPath() + "/../resources/rutas_arequipa.json";

    QByteArray jsonData;
    bool archivoEncontrado = false;

    for (const auto& ruta : posiblesRutas) {
        QFile file(ruta);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            jsonData = file.readAll();
            file.close();
            archivoEncontrado = true;
            break;
        }
    }

    if (!archivoEncontrado) {
        qWarning() << "[ServicioRutas] No se pudo encontrar el archivo de rutas viales en los directorios conocidos.";
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "[ServicioRutas] Error al parsear JSON de rutas:" << error.errorString();
        return false;
    }

    m_rutas.clear();
    QJsonObject rootObj = doc.object();

    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        QString claveOriginal = it.key();
        QJsonObject info = it.value().toObject();

        RutaUrbana r;
        r.distanciaMetros = info.value("distancia_m").toDouble(0.0);
        r.duracionSegundos = info.value("duracion_s").toDouble(0.0);
        r.encontrada = true;

        QJsonArray ptsArr = info.value("puntos").toArray();
        r.waypoints.reserve(ptsArr.size());
        for (const auto& pVal : ptsArr) {
            QJsonArray coord = pVal.toArray();
            if (coord.size() >= 2) {
                r.waypoints.append(QPointF(coord[0].toDouble(), coord[1].toDouble()));
            }
        }

        // Guardar tanto con clave exacta como normalizada
        m_rutas[claveOriginal] = r;
        QStringList partes = claveOriginal.split('|');
        if (partes.size() == 2) {
            QString claveNorm = generarClave(partes[0], partes[1]);
            if (!m_rutas.contains(claveNorm)) {
                m_rutas[claveNorm] = r;
            }
        }
    }

    m_inicializado = !m_rutas.isEmpty();
    qInfo() << "[ServicioRutas] Rutas viales cargadas con éxito:" << m_rutas.size() << "entradas disponibles.";
    return m_inicializado;
}

bool ServicioRutas::tieneRuta(const QString& origen, const QString& destino) const {
    QString clave = generarClave(origen, destino);
    if (m_rutas.contains(clave)) return true;
    if (m_rutas.contains(origen + "|" + destino)) return true;
    return false;
}

RutaUrbana ServicioRutas::obtenerRuta(const QString& origen, const QString& destino) const {
    // 1. Búsqueda directa por clave normalizada
    QString claveNorm = generarClave(origen, destino);
    if (m_rutas.contains(claveNorm)) {
        return m_rutas.value(claveNorm);
    }

    // 2. Búsqueda por clave literal
    QString claveLiteral = origen + "|" + destino;
    if (m_rutas.contains(claveLiteral)) {
        return m_rutas.value(claveLiteral);
    }

    // 3. Búsqueda aproximada / tolerante a prefijos
    for (auto it = m_rutas.begin(); it != m_rutas.end(); ++it) {
        QStringList partes = it.key().split('|');
        if (partes.size() == 2) {
            bool origMatch = (partes[0].contains(origen.left(6), Qt::CaseInsensitive) ||
                              origen.contains(partes[0].left(6), Qt::CaseInsensitive));
            bool destMatch = (partes[1].contains(destino.left(6), Qt::CaseInsensitive) ||
                              destino.contains(partes[1].left(6), Qt::CaseInsensitive));
            if (origMatch && destMatch) {
                return it.value();
            }
        }
    }

    // Si no se encuentra, retornar ruta con indicador encontrada = false
    RutaUrbana noEncontrada;
    noEncontrada.encontrada = false;
    noEncontrada.distanciaMetros = 3000.0;
    noEncontrada.duracionSegundos = 360.0;
    return noEncontrada;
}

double ServicioRutas::calcularDistanciaKm(const QString& origen, const QString& destino) const {
    auto r = obtenerRuta(origen, destino);
    if (r.encontrada && r.distanciaMetros > 0.0) {
        return r.distanciaMetros / 1000.0;
    }
    return 3.5; // Estimación urbana por defecto
}

double ServicioRutas::calcularDuracionMin(const QString& origen, const QString& destino) const {
    auto r = obtenerRuta(origen, destino);
    if (r.encontrada && r.duracionSegundos > 0.0) {
        return r.duracionSegundos / 60.0;
    }
    return 8.0; // Minutos promedio
}

} // namespace siger
