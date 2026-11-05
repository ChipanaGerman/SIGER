#pragma once
/**
 * @file EstadisticasView.h
 * @brief Vista de métricas y gráficos estadísticos de entregas con QtCharts.
 */

#include <QWidget>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>

namespace siger {

class EstadisticasView : public QWidget {
    Q_OBJECT

public:
    explicit EstadisticasView(QWidget* parent = nullptr);
    ~EstadisticasView() override = default;

public slots:
    void slotActualizarEstadisticas(int total, int pendientes, int completados, int cancelados);

private:
    void setupUi();

    QLabel*              m_lblTotal{nullptr};
    QLabel*              m_lblPendientes{nullptr};
    QLabel*              m_lblCompletados{nullptr};
    QLabel*              m_lblCancelados{nullptr};
    QLabel*              m_lblEfectividad{nullptr};

    QChart*              m_chart{nullptr};
    QPieSeries*          m_series{nullptr};
    QChartView*          m_chartView{nullptr};
};

} // namespace siger
