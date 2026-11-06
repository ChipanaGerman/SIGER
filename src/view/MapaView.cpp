/**
 * @file MapaView.cpp
 * @brief Implementación de la vista de mapa interactivo con QGraphicsScene.
 */

#include "MapaView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QResizeEvent>
#include <cmath>

namespace siger {

MapaView::MapaView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void MapaView::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // Barra de información superior
    auto topLayout = new QHBoxLayout();
    m_lblInfo = new QLabel("🗺️ Red de Reparto Metropolitano Arequipa | Envíos activos: 0", this);
    m_lblInfo->setStyleSheet("font-weight: bold; font-size: 13px; color: #1565C0; padding: 4px;");
    topLayout->addWidget(m_lblInfo);
    topLayout->addStretch();

    auto lblLeyenda = new QLabel("🟢 En Tránsito  |  🔵 Nodo Logístico  |  🟠 Ruta Activa", this);
    lblLeyenda->setStyleSheet("font-size: 11px; color: #555;");
    topLayout->addWidget(lblLeyenda);

    layout->addLayout(topLayout);

    // Escena y Vista
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-380, -260, 760, 520);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setBackgroundBrush(QBrush(QColor(248, 250, 252)));
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(m_view);

    inicializarNodos();

    m_timerAnim = new QTimer(this);
    connect(m_timerAnim, &QTimer::timeout, this, &MapaView::onTimerAnimacion);
    m_timerAnim->start(30); // ~33 fps
}

void MapaView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_view && m_scene) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MapaView::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (m_view && m_scene) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MapaView::inicializarNodos() {
    // 10 puntos clave de Arequipa (coordenadas balanceadas en la escena)
    m_posiciones["Plaza de Armas"]          = QPointF(0, 10);
    m_posiciones["Av. Ejercito 450"]        = QPointF(-160, -80);
    m_posiciones["Calle Mercaderes 120"]    = QPointF(55, -20);
    m_posiciones["Av. Dolores 890"]         = QPointF(160, 110);
    m_posiciones["Urb. Umacollo B-12"]      = QPointF(-180, 50);
    m_posiciones["Av. Cayma 310"]           = QPointF(-200, -170);
    m_posiciones["Parque Industrial"]       = QPointF(230, -90);
    m_posiciones["Av. Lambramani 105"]      = QPointF(140, 20);
    m_posiciones["Calle San Francisco 204"] = QPointF(25, -90);
    m_posiciones["Av. Venezuela 800 (UNSA)"]= QPointF(70, 150);

    // Malla de calles conectadas (líneas punteadas sutiles)
    QPen penCalle(QColor(218, 225, 233), 1.5, Qt::DashLine);
    QList<QPointF> puntos = m_posiciones.values();
    for (int i = 0; i < puntos.size(); ++i) {
        for (int j = i + 1; j < puntos.size(); ++j) {
            double dist = QLineF(puntos[i], puntos[j]).length();
            if (dist < 185.0) {
                m_scene->addLine(QLineF(puntos[i], puntos[j]), penCalle);
            }
        }
    }

    // Dibujar nodos con estilo
    QFont font("Segoe UI", 8, QFont::Bold);
    for (auto it = m_posiciones.begin(); it != m_posiciones.end(); ++it) {
        QPointF pt = it.value();

        // Anillo exterior
        m_scene->addEllipse(pt.x() - 11, pt.y() - 11, 22, 22,
                            QPen(QColor(25, 118, 210, 120), 2),
                            QBrush(QColor(227, 242, 253)));

        // Círculo central
        m_scene->addEllipse(pt.x() - 6, pt.y() - 6, 12, 12,
                            QPen(Qt::white, 1.5),
                            QBrush(QColor(25, 118, 210)));

        // Etiqueta del nodo
        auto text = m_scene->addText(it.key(), font);
        text->setDefaultTextColor(QColor(33, 43, 54));
        text->setPos(pt.x() - 45, pt.y() + 12);
    }
}

QPointF MapaView::getPunto(const QString& nombre) const {
    if (m_posiciones.contains(nombre)) {
        return m_posiciones[nombre];
    }
    // Búsqueda flexible tolerante a tildes o variaciones de nombre
    for (auto it = m_posiciones.begin(); it != m_posiciones.end(); ++it) {
        if (it.key().contains(nombre.left(6), Qt::CaseInsensitive) ||
            nombre.contains(it.key().left(6), Qt::CaseInsensitive)) {
            return it.value();
        }
    }
    return QPointF(0, 0); // Plaza de Armas por defecto
}

void MapaView::animarEntrega(const QString& idPedido, const QString& origen, const QString& destino, const QString& /*repartidor*/) {
    QPointF p1 = getPunto(origen);
    QPointF p2 = getPunto(destino);

    if (p1 == p2) {
        p2 = QPointF(p1.x() + 80, p1.y() + 60);
    }

    // Línea activa de reparto
    QPen penRuta(QColor(255, 112, 67, 220), 3, Qt::SolidLine, Qt::RoundCap);
    auto linea = m_scene->addLine(QLineF(p1, p2), penRuta);

    // Móvil en movimiento (círculo verde/naranja llamativo)
    auto movil = m_scene->addEllipse(p1.x() - 8, p1.y() - 8, 16, 16,
                                     QPen(Qt::white, 2),
                                     QBrush(QColor(46, 125, 50)));

    // Etiqueta flotante con ID del pedido
    QFont fontId("Segoe UI", 8, QFont::Bold);
    auto etiqueta = m_scene->addText(idPedido.isEmpty() ? "🚚 En Ruta" : idPedido + " 🚚", fontId);
    etiqueta->setDefaultTextColor(QColor(230, 81, 0));
    etiqueta->setPos(p1.x() - 25, p1.y() - 24);

    MovilAnimado anim;
    anim.idPedido = idPedido;
    anim.origen = p1;
    anim.destino = p2;
    anim.progreso = 0.0;
    // Velocidad suave: tarda unos 2 a 3 segundos en cruzar
    anim.velocidad = 0.012;
    anim.item = movil;
    anim.linea = linea;
    anim.etiqueta = etiqueta;

    m_moviles.push_back(anim);

    m_lblInfo->setText(QString("🗺️ Red de Reparto Metropolitano Arequipa | Envíos activos: %1").arg(m_moviles.size()));
}

void MapaView::animarEntregaSimple(const QString& origen, const QString& destino) {
    animarEntrega("", origen, destino, "");
}

void MapaView::onTimerAnimacion() {
    for (auto it = m_moviles.begin(); it != m_moviles.end();) {
        it->progreso += it->velocidad;
        if (it->progreso >= 1.0) {
            m_scene->removeItem(it->item);
            m_scene->removeItem(it->linea);
            m_scene->removeItem(it->etiqueta);
            delete it->item;
            delete it->linea;
            delete it->etiqueta;
            it = m_moviles.erase(it);
        } else {
            double x = it->origen.x() + (it->destino.x() - it->origen.x()) * it->progreso;
            double y = it->origen.y() + (it->destino.y() - it->origen.y()) * it->progreso;
            it->item->setRect(x - 8, y - 8, 16, 16);
            it->etiqueta->setPos(x - 25, y - 24);
            ++it;
        }
    }
    m_lblInfo->setText(QString("🗺️ Red de Reparto Metropolitano Arequipa | Envíos activos: %1").arg(m_moviles.size()));
}

void MapaView::resetearMapa() {
    for (auto& m : m_moviles) {
        m_scene->removeItem(m.item);
        m_scene->removeItem(m.linea);
        m_scene->removeItem(m.etiqueta);
        delete m.item;
        delete m.linea;
        delete m.etiqueta;
    }
    m_moviles.clear();
    m_lblInfo->setText("🗺️ Red de Reparto Metropolitano Arequipa | Envíos activos: 0");
}

} // namespace siger
