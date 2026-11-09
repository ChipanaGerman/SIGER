/**
 * @file MapaView.cpp
 * @brief Implementación de la vista de mapa interactivo con fondo urbano de Arequipa,
 *        rutas vehiculares reales por calles y QGraphicsScene.
 */

#include "MapaView.h"
#include "ServicioRutas.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QFile>
#include <QCoreApplication>
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

    // Barra de información y controles superiores estilo Google Maps
    auto topLayout = new QHBoxLayout();
    m_lblInfo = new QLabel("🗺️ Red de Reparto Metropolitano Arequipa (Rutas Viales OSRM) | Envíos activos: 0", this);
    m_lblInfo->setStyleSheet("font-weight: bold; font-size: 13px; color: #1A73E8; padding: 4px;");
    topLayout->addWidget(m_lblInfo);
    topLayout->addStretch();

    auto btnZoomIn = new QPushButton("🔍 +", this);
    btnZoomIn->setToolTip("Acercar mapa");
    btnZoomIn->setStyleSheet("font-weight: bold; padding: 4px 10px; background: white; border: 1px solid #DADCE0; border-radius: 4px;");
    connect(btnZoomIn, &QPushButton::clicked, this, &MapaView::zoomIn);
    topLayout->addWidget(btnZoomIn);

    auto btnZoomOut = new QPushButton("🔍 -", this);
    btnZoomOut->setToolTip("Alejar mapa");
    btnZoomOut->setStyleSheet("font-weight: bold; padding: 4px 10px; background: white; border: 1px solid #DADCE0; border-radius: 4px;");
    connect(btnZoomOut, &QPushButton::clicked, this, &MapaView::zoomOut);
    topLayout->addWidget(btnZoomOut);

    auto btnCentrar = new QPushButton("🎯 Centrar", this);
    btnCentrar->setToolTip("Centrar en Centro Histórico de Arequipa");
    btnCentrar->setStyleSheet("font-weight: bold; padding: 4px 10px; background: white; border: 1px solid #DADCE0; border-radius: 4px; color: #1A73E8;");
    connect(btnCentrar, &QPushButton::clicked, this, &MapaView::centrarMapa);
    topLayout->addWidget(btnCentrar);

    layout->addLayout(topLayout);

    // Escena y Vista con soporte interactivo tipo Google Maps (arrastre con mouse y zoom)
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-768, -768, 1536, 1536);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setBackgroundBrush(QBrush(QColor(241, 243, 244))); // Fondo neutro Maps
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);         // Arrastrar con mano como Google Maps
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(m_view);

    inicializarNodos();

    m_timerAnim = new QTimer(this);
    connect(m_timerAnim, &QTimer::timeout, this, &MapaView::onTimerAnimacion);
    m_timerAnim->start(30); // ~33 fps
}

void MapaView::zoomIn() {
    if (m_view) {
        m_view->scale(1.25, 1.25);
    }
}

void MapaView::zoomOut() {
    if (m_view) {
        m_view->scale(0.8, 0.8);
    }
}

void MapaView::centrarMapa() {
    if (m_view && m_scene) {
        m_view->resetTransform();
        m_view->fitInView(QRectF(-400, -350, 800, 700), Qt::KeepAspectRatio);
    }
}

void MapaView::wheelEvent(QWheelEvent* event) {
    if (m_view) {
        double factor = (event->angleDelta().y() > 0) ? 1.15 : 0.85;
        m_view->scale(factor, factor);
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

void MapaView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    static bool primerResize = true;
    if (primerResize && m_view && m_scene) {
        primerResize = false;
        centrarMapa();
    }
}

void MapaView::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    centrarMapa();
}

void MapaView::inicializarNodos() {
    // 1. Cargar fondo de mapa urbano real de Arequipa
    QPixmap mapaPix;
    if (QFile::exists(":/resources/mapa_arequipa.png")) {
        mapaPix.load(":/resources/mapa_arequipa.png");
    } else if (QFile::exists("resources/mapa_arequipa.png")) {
        mapaPix.load("resources/mapa_arequipa.png");
    } else if (QFile::exists(QCoreApplication::applicationDirPath() + "/resources/mapa_arequipa.png")) {
        mapaPix.load(QCoreApplication::applicationDirPath() + "/resources/mapa_arequipa.png");
    } else if (QFile::exists(QCoreApplication::applicationDirPath() + "/../resources/mapa_arequipa.png")) {
        mapaPix.load(QCoreApplication::applicationDirPath() + "/../resources/mapa_arequipa.png");
    }

    if (!mapaPix.isNull()) {
        auto mapaItem = m_scene->addPixmap(mapaPix);
        mapaItem->setPos(-768, -768);
        mapaItem->setZValue(-100);
        mapaItem->setOpacity(0.94);
    }

    // 2. 20 Nodos Logísticos de Arequipa georreferenciados con precisión métrica
    m_posiciones["Plaza de Armas"]                      = QPointF(70.7, -68.9);
    m_posiciones["Calle Mercaderes 120"]                = QPointF(95.2, -78.6);
    m_posiciones["Calle San Francisco 204"]             = QPointF(92.8, -105.3);
    m_posiciones["Mirador de Yanahuara"]                = QPointF(9.0, -197.6);
    m_posiciones["Av. Ejercito 450"]                    = QPointF(-17.8, -161.2);
    m_posiciones["Av. Cayma 310"]                       = QPointF(-52.8, -254.7);
    m_posiciones["Urb. Umacollo B-12"]                  = QPointF(-47.0, -23.9);
    m_posiciones["UNSA Ingenierias (Av. Independencia)"] = QPointF(174.4, 8.9);
    m_posiciones["Av. Venezuela 800 (UNSA)"]            = QPointF(250.1, 61.1);
    m_posiciones["Av. Lambramani 105"]                  = QPointF(246.6, 142.5);
    m_posiciones["Mall Aventura Porongoche"]            = QPointF(355.0, 259.1);
    m_posiciones["Av. Dolores 890"]                     = QPointF(211.7, 206.8);
    m_posiciones["Plaza Mayta Capac (Miraflores)"]      = QPointF(290.9, -145.4);
    m_posiciones["Parque Selva Alegre"]                 = QPointF(133.6, -206.1);
    m_posiciones["Mirador de Sachaca"]                  = QPointF(-250.9, 251.8);
    m_posiciones["Terminal Terrestre Arequipa"]         = QPointF(-72.6, 279.7);
    m_posiciones["Cerro Colorado (Plaza Las Americas)"] = QPointF(-196.1, -473.3);
    m_posiciones["Parque Industrial"]                   = QPointF(-483.9, -603.2);
    m_posiciones["Plaza de Paucarpata"]                 = QPointF(518.1, 344.1);
    m_posiciones["Molino de Sabandia"]                  = QPointF(640.4, 577.3);

    // 3. Inicializar servicio de rutas viales reales (OSRM)
    ServicioRutas::instancia().inicializar();

    // 4. Malla vial conectada sutil
    QPen penCalle(QColor(120, 144, 156, 110), 1.6, Qt::DashLine, Qt::RoundCap);
    QList<QPointF> puntos = m_posiciones.values();
    for (int i = 0; i < puntos.size(); ++i) {
        for (int j = i + 1; j < puntos.size(); ++j) {
            double dist = QLineF(puntos[i], puntos[j]).length();
            if (dist < 195.0) {
                auto line = m_scene->addLine(QLineF(puntos[i], puntos[j]), penCalle);
                line->setZValue(-50);
            }
        }
    }

    // 5. Dibujar pines de ubicación estilo Google Maps
    QFont font("Segoe UI", 8, QFont::Bold);

    for (auto it = m_posiciones.begin(); it != m_posiciones.end(); ++it) {
        QPointF pt = it.value();

        // Sombra / halo exterior
        auto halo = m_scene->addEllipse(pt.x() - 10, pt.y() - 10, 20, 20,
                                        QPen(QColor(26, 115, 232, 90), 2.5),
                                        QBrush(QColor(232, 240, 254, 210)));
        halo->setZValue(1);

        // Pin central tipo Google Maps
        auto pin = m_scene->addEllipse(pt.x() - 5.5, pt.y() - 5.5, 11, 11,
                                       QPen(Qt::white, 1.8),
                                       QBrush(QColor(234, 67, 53)));
        if (it.key().contains("UNSA") || it.key().contains("Plaza de Armas")) {
            pin->setBrush(QBrush(QColor(26, 115, 232)));
        }
        pin->setZValue(2);

        // Etiqueta estilo Google Maps "pill" con fondo claro
        auto text = m_scene->addText(it.key(), font);
        text->setDefaultTextColor(QColor(32, 33, 36));
        text->setPos(pt.x() - 40, pt.y() + 8);
        text->setZValue(3);
    }
}

QPointF MapaView::getPunto(const QString& nombre) const {
    if (m_posiciones.contains(nombre)) {
        return m_posiciones[nombre];
    }
    for (auto it = m_posiciones.begin(); it != m_posiciones.end(); ++it) {
        if (it.key().contains(nombre.left(6), Qt::CaseInsensitive) ||
            nombre.contains(it.key().left(6), Qt::CaseInsensitive)) {
            return it.value();
        }
    }
    return m_posiciones.value("Plaza de Armas", QPointF(70.7, -68.9));
}

void MapaView::animarEntrega(const QString& idPedido,
                            const QString& origen,
                            const QString& destino,
                            const QString& repartidor,
                            const QString& tipoVehiculo) {
    // 1. Obtener la ruta vial real por calles respetando la topología de Arequipa (OSRM)
    auto rutaUrbana = ServicioRutas::instancia().obtenerRuta(origen, destino);
    QVector<QPointF> puntos = rutaUrbana.waypoints;

    if (puntos.size() < 2) {
        QPointF p1 = getPunto(origen);
        QPointF p2 = getPunto(destino);
        if (p1 == p2) {
            p2 = QPointF(p1.x() + 80, p1.y() + 60);
        }
        puntos = { p1, p2 };
    }

    // Configurar color, icono y velocidad de avance constante según el tipo de vehículo
    QColor colorRuta(251, 140, 0, 230); // Default naranja (Auto)
    QColor colorMovil(230, 81, 0);       // Naranja intenso
    QString icono = "🚗";
    double velPxPorFrame = 2.0;          // Velocidad constante equilibrada (~7-9 seg)

    if (tipoVehiculo.contains("Moto", Qt::CaseInsensitive)) {
        colorRuta = QColor(2, 136, 209, 235);  // Celeste brillante
        colorMovil = QColor(1, 87, 155);       // Azul oscuro
        icono = "🏍️";
        velPxPorFrame = 2.8;                   // Más rápida y ágil (~6 seg)
    } else if (tipoVehiculo.contains("Camion", Qt::CaseInsensitive) || tipoVehiculo.contains("Camión", Qt::CaseInsensitive)) {
        colorRuta = QColor(142, 36, 170, 235); // Púrpura
        colorMovil = QColor(106, 27, 154);     // Púrpura oscuro
        icono = "🚚";
        velPxPorFrame = 1.4;                   // Más pesado y prudente (~11 seg)
    }

    // 2. Calcular distancias acumuladas a lo largo de la polilínea de calles
    QVector<double> distAcum;
    distAcum.reserve(puntos.size());
    distAcum.push_back(0.0);
    double distTotal = 0.0;
    for (int i = 1; i < puntos.size(); ++i) {
        double d = QLineF(puntos[i - 1], puntos[i]).length();
        distTotal += d;
        distAcum.push_back(distTotal);
    }
    if (distTotal <= 0.001) {
        distTotal = 1.0;
    }

    // 3. Trazar la polilínea de carretera estilo navegación GPS (Google Maps polyline)
    QPainterPath pathRuta;
    pathRuta.moveTo(puntos.first());
    for (int i = 1; i < puntos.size(); ++i) {
        pathRuta.lineTo(puntos[i]);
    }

    QPen penRuta(colorRuta, 4.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    auto lineaPath = m_scene->addPath(pathRuta, penRuta);
    lineaPath->setZValue(10);

    // 4. Móvil en movimiento (inicia en el punto exacto de origen)
    QPointF pInicio = puntos.first();
    auto movil = m_scene->addEllipse(pInicio.x() - 9, pInicio.y() - 9, 18, 18,
                                     QPen(Qt::white, 2.5),
                                     QBrush(colorMovil));
    movil->setZValue(20);

    // 5. Etiqueta flotante con ID del pedido e icono del vehículo
    QFont fontId("Segoe UI", 9, QFont::Bold);
    QString textoEtiqueta = idPedido.isEmpty() ? (icono + " En Ruta") : (icono + " " + idPedido);
    if (!repartidor.isEmpty()) {
        textoEtiqueta += " (" + repartidor + ")";
    }
    auto etiqueta = m_scene->addText(textoEtiqueta, fontId);
    etiqueta->setDefaultTextColor(colorMovil);
    etiqueta->setPos(pInicio.x() - 25, pInicio.y() - 26);
    etiqueta->setZValue(25);

    // 6. Registrar en la lista de móviles activos
    MovilAnimado anim;
    anim.idPedido = idPedido;
    anim.rutaPuntos = puntos;
    anim.distAcumulada = distAcum;
    anim.longitudTotal = distTotal;
    anim.distanciaRecorrida = 0.0;
    anim.velocidadPixelesPorFrame = velPxPorFrame;
    anim.item = movil;
    anim.linea = lineaPath;
    anim.etiqueta = etiqueta;

    m_moviles.push_back(anim);

    m_lblInfo->setText(QString("🗺️ Red de Reparto Metropolitano Arequipa (Rutas Viales OSRM) | Envíos activos: %1").arg(m_moviles.size()));
}

void MapaView::animarEntregaSimple(const QString& origen, const QString& destino) {
    animarEntrega("", origen, destino, "", "");
}

void MapaView::onTimerAnimacion() {
    for (auto it = m_moviles.begin(); it != m_moviles.end();) {
        it->distanciaRecorrida += it->velocidadPixelesPorFrame;
        if (it->distanciaRecorrida >= it->longitudTotal) {
            // Completó el trayecto por las calles
            m_scene->removeItem(it->item);
            m_scene->removeItem(it->linea);
            m_scene->removeItem(it->etiqueta);
            delete it->item;
            delete it->linea;
            delete it->etiqueta;
            it = m_moviles.erase(it);
        } else {
            // Movimiento cinemático uniforme a lo largo de las curvas y esquinas
            double dRec = it->distanciaRecorrida;
            const auto& distAcum = it->distAcumulada;
            const auto& pts = it->rutaPuntos;

            int segIdx = 0;
            while (segIdx + 1 < distAcum.size() && distAcum[segIdx + 1] < dRec) {
                segIdx++;
            }

            double dSeg = distAcum[segIdx + 1] - distAcum[segIdx];
            double t = (dSeg > 1e-4) ? ((dRec - distAcum[segIdx]) / dSeg) : 0.0;
            if (t < 0.0) t = 0.0;
            if (t > 1.0) t = 1.0;

            QPointF pA = pts[segIdx];
            QPointF pB = pts[segIdx + 1];
            QPointF posActual = pA + (pB - pA) * t;

            it->item->setRect(posActual.x() - 9, posActual.y() - 9, 18, 18);
            it->etiqueta->setPos(posActual.x() - 25, posActual.y() - 26);
            ++it;
        }
    }
    m_lblInfo->setText(QString("🗺️ Red de Reparto Metropolitano Arequipa (Rutas Viales OSRM) | Envíos activos: %1").arg(m_moviles.size()));
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
    m_lblInfo->setText("🗺️ Red de Reparto Metropolitano Arequipa (Rutas Viales OSRM) | Envíos activos: 0");
}

} // namespace siger
