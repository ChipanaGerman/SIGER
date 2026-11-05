#pragma once
/**
 * @file MapaView.h
 * @brief Representación gráfica interactiva 2D del mapa de entregas con QGraphicsScene.
 */

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QMap>
#include <QPointF>

namespace siger {

class MapaView : public QWidget {
    Q_OBJECT

public:
    explicit MapaView(QWidget* parent = nullptr);
    ~MapaView() override = default;

public slots:
    void animarEntrega(const QString& origen, const QString& destino);
    void resetearMapa();

private slots:
    void onTimerAnimacion();

private:
    void setupUi();
    void inicializarNodos();

    QGraphicsView*  m_view{nullptr};
    QGraphicsScene* m_scene{nullptr};
    QTimer*         m_timerAnim{nullptr};

    struct NodoMapa {
        QString nombre;
        QPointF pos;
    };
    QMap<QString, QPointF> m_posiciones;

    struct MovilAnimado {
        QPointF origen;
        QPointF destino;
        double progreso{0.0};
        QGraphicsEllipseItem* item{nullptr};
        QGraphicsLineItem*    linea{nullptr};
    };
    std::vector<MovilAnimado> m_moviles;
};

} // namespace siger
