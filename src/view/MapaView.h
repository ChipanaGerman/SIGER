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
#include <QLabel>
#include <vector>

namespace siger {

class MapaView : public QWidget {
    Q_OBJECT

public:
    explicit MapaView(QWidget* parent = nullptr);
    ~MapaView() override = default;

public slots:
    void animarEntrega(const QString& idPedido, const QString& origen, const QString& destino, const QString& repartidor = "");
    void animarEntregaSimple(const QString& origen, const QString& destino);
    void resetearMapa();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

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

    struct MovilAnimado {
        QString idPedido;
        QPointF origen;
        QPointF destino;
        double progreso{0.0};
        double velocidad{0.015};
        QGraphicsEllipseItem* item{nullptr};
        QGraphicsLineItem*    linea{nullptr};
        QGraphicsTextItem*    etiqueta{nullptr};
    };
    std::vector<MovilAnimado> m_moviles;
};

} // namespace siger
