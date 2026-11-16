#pragma once
/**
 * @file MapaView.h
 * @brief Representación gráfica interactiva 2D del mapa de entregas con QGraphicsScene y rutas viales reales.
 */

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <QTimer>
#include <QMap>
#include <QPointF>
#include <QVector>
#include <QLabel>
#include <vector>

namespace siger {

class MapaView : public QWidget {
    Q_OBJECT

public:
    explicit MapaView(QWidget* parent = nullptr);
    ~MapaView() override = default;

public slots:
    void animarEntrega(const QString& idPedido,
                       const QString& origen,
                       const QString& destino,
                       const QString& repartidor = "",
                       const QString& tipoVehiculo = "");
    void animarEntregaSimple(const QString& origen, const QString& destino);
    void resetearMapa();
    void setFactorVelocidad(double factor);
    void setFactorTrafico(double factor);
    void setModoOscuro(bool activo);
    void actualizarHud(int enCola, int enRuta, const QString& traficoStr, const QString& tiempoStr);
    void zoomIn();
    void zoomOut();
    void centrarMapa();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onTimerAnimacion();

private:
    void setupUi();
    void inicializarNodos();
    QPointF getPunto(const QString& nombre) const;

    QGraphicsView*  m_view{nullptr};
    QGraphicsScene* m_scene{nullptr};
    QTimer*         m_timerAnim{nullptr};
    QLabel*         m_lblInfo{nullptr};

    QMap<QString, QPointF> m_posiciones;
    double                 m_factorVelocidad{1.0};
    double                 m_factorTrafico{1.0};
    bool                   m_esModoOscuro{false};

    // Telemetría Mini-HUD flotante
    class QFrame*          m_hudOverlay{nullptr};
    QLabel*                m_lblHudCola{nullptr};
    QLabel*                m_lblHudRuta{nullptr};
    QLabel*                m_lblHudTrafico{nullptr};
    QLabel*                m_lblHudTiempo{nullptr};

    struct PulsoLlegada {
        QPointF centro;
        double radioActual{8.0};
        double radioMax{48.0};
        double opacidad{1.0};
        QGraphicsEllipseItem* item{nullptr};
    };
    std::vector<PulsoLlegada> m_pulsos;

    struct MovilAnimado {
        QString                 idPedido;
        QVector<QPointF>        rutaPuntos;
        QVector<double>         distAcumulada;
        double                  longitudTotal{0.0};
        double                  distanciaRecorrida{0.0};
        double                  velocidadPixelesPorFrame{2.2};
        QGraphicsEllipseItem*   item{nullptr};
        QGraphicsPolygonItem*   flechaDireccion{nullptr};
        QGraphicsPathItem*      lineaPendiente{nullptr};
        QGraphicsPathItem*      lineaRecorrida{nullptr};
        QGraphicsTextItem*      etiqueta{nullptr};
        QPen                    penRecorrida;
    };
    std::vector<MovilAnimado> m_moviles;
};

} // namespace siger
