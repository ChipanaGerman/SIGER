/**
 * @file MapaView.cpp
 * @brief Implementación de la vista de mapa interactivo con QGraphicsScene.
 */

#include "MapaView.h"
#include <QVBoxLayout>
#include <QPen>
#include <QBrush>
#include <QFont>

namespace siger {

MapaView::MapaView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void MapaView::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-350, -250, 700, 500);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setBackgroundBrush(QBrush(QColor(245, 247, 250)));
    layout->addWidget(m_view);

    inicializarNodos();

    m_timerAnim = new QTimer(this);
    connect(m_timerAnim, &QTimer::timeout, this, &MapaView::onTimerAnimacion);
    m_timerAnim->start(30); // ~33 fps
}

void MapaView::inicializarNodos() {
    m_posiciones["Plaza de Armas"]          = QPointF(0, 0);
    m_posiciones["Av. Ejército 450"]         = QPointF(-160, -120);
    m_posiciones["Calle Mercaderes 120"]     = QPointF(50, -30);
    m_posiciones["Av. Dolores 890"]          = QPointF(180, 120);
    m_posiciones["Urb. Umacollo B-12"]       = QPointF(-200, 40);
    m_posiciones["Av. Cayma 310"]            = QPointF(-220, -180);
    m_posiciones["Parque Industrial"]        = QPointF(240, -100);
    m_posiciones["Av. Lambramani 105"]       = QPointF(150, 20);
    m_posiciones["Calle San Francisco 204"]  = QPointF(20, -90);
    m_posiciones["Av. Venezuela 800 (UNSA)"] = QPointF(80, 160);

    // Dibujar rutas base (malla de conectividad)
    QPen penRuta(QColor(200, 210, 220), 2, Qt::DashLine);
    QList<QPointF> puntos = m_posiciones.values();
    for (int i = 0; i < puntos.size(); ++i) {
        for (int j = i + 1; j < puntos.size(); ++j) {
            double dist = QLineF(puntos[i], puntos[j]).length();
            if (dist < 200.0) { // Conectar puntos cercanos
                m_scene->addLine(QLineF(puntos[i], puntos[j]), penRuta);
            }
        }
    }

    // Dibujar nodos
    QFont font("Arial", 8, QFont::Bold);
    for (auto it = m_posiciones.begin(); it != m_posiciones.end(); ++it) {
        QPointF pt = it.value();
        // Círculo del nodo
        m_scene->addEllipse(pt.x() - 10, pt.y() - 10, 20, 20,
                            QPen(QColor(25, 118, 210), 2),
                            QBrush(QColor(66, 165, 245)));

        // Etiqueta
        auto text = m_scene->addText(it.key(), font);
        text->setDefaultTextColor(QColor(33, 33, 33));
        text->setPos(pt.x() - 40, pt.y() + 12);
    }
}

void MapaView::animarEntrega(const QString& origen, const QString& destino) {
    if (!m_posiciones.contains(origen) || !m_posiciones.contains(destino)) return;

    QPointF p1 = m_posiciones[origen];
    QPointF p2 = m_posiciones[destino];

    // Línea activa de reparto
    auto linea = m_scene->addLine(QLineF(p1, p2), QPen(QColor(255, 152, 0, 180), 3));

    // Vehículo en movimiento
    auto movil = m_scene->addEllipse(p1.x() - 6, p1.y() - 6, 12, 12,
                                     QPen(Qt::white, 1.5),
                                     QBrush(QColor(230, 81, 0)));

    MovilAnimado anim;
    anim.origen = p1;
    anim.destino = p2;
    anim.progreso = 0.0;
    anim.item = movil;
    anim.linea = linea;

    m_moviles.push_back(anim);
}

void MapaView::onTimerAnimacion() {
    for (auto it = m_moviles.begin(); it != m_moviles.end();) {
        it->progreso += 0.02; // Incremento de velocidad de animación
        if (it->progreso >= 1.0) {
            m_scene->removeItem(it->item);
            m_scene->removeItem(it->linea);
            delete it->item;
            delete it->linea;
            it = m_moviles.erase(it);
        } else {
            double x = it->origen.x() + (it->destino.x() - it->origen.x()) * it->progreso;
            double y = it->origen.y() + (it->destino.y() - it->origen.y()) * it->progreso;
            it->item->setRect(x - 6, y - 6, 12, 12);
            ++it;
        }
    }
}

void MapaView::resetearMapa() {
    for (auto& m : m_moviles) {
        m_scene->removeItem(m.item);
        m_scene->removeItem(m.linea);
        delete m.item;
        delete m.linea;
    }
    m_moviles.clear();
}

} // namespace siger
