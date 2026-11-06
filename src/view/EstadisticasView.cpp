/**
 * @file EstadisticasView.cpp
 * @brief Implementación de la vista de métricas y gráficos estadísticos.
 */

#include "EstadisticasView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

namespace siger {

EstadisticasView::EstadisticasView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void EstadisticasView::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // Panel superior de tarjetas KPI
    auto kpiLayout = new QGridLayout();

    auto crearTarjeta = [](const QString& titulo, const QString& valorInicial, const QString& colorHex) -> std::pair<QGroupBox*, QLabel*> {
        auto grp = new QGroupBox(titulo);
        auto lyt = new QVBoxLayout(grp);
        auto lbl = new QLabel(valorInicial);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(colorHex));
        lyt->addWidget(lbl);
        return {grp, lbl};
    };

    auto [grpTot, lblTot] = crearTarjeta("Total Pedidos", "0", "#1976D2");
    auto [grpPen, lblPen] = crearTarjeta("Pendientes", "0", "#FFA000");
    auto [grpCom, lblCom] = crearTarjeta("Completados", "0", "#388E3C");
    auto [grpCan, lblCan] = crearTarjeta("Cancelados", "0", "#D32F2F");
    auto [grpEfe, lblEfe] = crearTarjeta("Tasa de Éxito", "0%", "#7B1FA2");

    m_lblTotal = lblTot;
    m_lblPendientes = lblPen;
    m_lblCompletados = lblCom;
    m_lblCancelados = lblCan;
    m_lblEfectividad = lblEfe;

    kpiLayout->addWidget(grpTot, 0, 0);
    kpiLayout->addWidget(grpPen, 0, 1);
    kpiLayout->addWidget(grpCom, 0, 2);
    kpiLayout->addWidget(grpCan, 0, 3);
    kpiLayout->addWidget(grpEfe, 0, 4);

    mainLayout->addLayout(kpiLayout);

    // Gráfico de pastel (QPieSeries)
    m_series = new QPieSeries(this);
    m_series->append("Pendientes", 1);
    m_series->append("Completados", 0);
    m_series->append("Cancelados", 0);

    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setTitle("Distribución de Estados de Pedidos");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->legend()->setAlignment(Qt::AlignRight);

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(m_chartView);
}

void EstadisticasView::slotActualizarEstadisticas(int total, int pendientes, int completados, int cancelados) {
    m_lblTotal->setText(QString::number(total));
    m_lblPendientes->setText(QString::number(pendientes));
    m_lblCompletados->setText(QString::number(completados));
    m_lblCancelados->setText(QString::number(cancelados));

    double tasa = 0.0;
    if (total > 0) {
        tasa = (static_cast<double>(completados) / total) * 100.0;
    }
    m_lblEfectividad->setText(QString::number(tasa, 'f', 1) + "%");

    m_series->clear();
    if (pendientes > 0) {
        auto slicePen = m_series->append("Pendientes (" + QString::number(pendientes) + ")", pendientes);
        slicePen->setColor(QColor("#FFA000"));
    }
    if (completados > 0) {
        auto sliceCom = m_series->append("Completados (" + QString::number(completados) + ")", completados);
        sliceCom->setColor(QColor("#388E3C"));
    }
    if (cancelados > 0) {
        auto sliceCan = m_series->append("Cancelados (" + QString::number(cancelados) + ")", cancelados);
        sliceCan->setColor(QColor("#D32F2F"));
    }

    if (m_series->count() == 0) {
        m_series->append("Sin datos", 1);
    }
}

} // namespace siger
