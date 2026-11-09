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
