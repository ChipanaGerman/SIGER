/**
 * @file FlotaView.cpp
 * @brief Implementación del panel visual de monitoreo en tiempo real de la flota.
 */

#include "FlotaView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFont>

namespace siger {

FlotaView::FlotaView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();

    m_timerProgreso = new QTimer(this);
    connect(m_timerProgreso, &QTimer::timeout, this, &FlotaView::onTimerAnimacionProgreso);
    m_timerProgreso->start(100);
}

void FlotaView::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(10);

    auto lblTitulo = new QLabel("🚚 Estado de la Flota Operativa en Tiempo Real", this);
    lblTitulo->setStyleSheet("font-size: 15px; font-weight: bold; color: #1A73E8; padding-bottom: 4px;");
    mainLayout->addWidget(lblTitulo);

    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto container = new QWidget(scroll);
    auto grid = new QGridLayout(container);
    grid->setSpacing(12);

    crearTarjeta("R-101", "Carlos Quispe (Moto 1)", "Moto",   "MTO-501", 15.0,   "🏍️");
    crearTarjeta("R-102", "Lucía Torres (Moto 2)",  "Moto",   "MTO-502", 15.0,   "🏍️");
    crearTarjeta("R-103", "María Flores (Auto 1)",  "Auto",   "AUT-701", 100.0,  "🚗");
    crearTarjeta("R-104", "Jorge Ramos (Auto 2)",   "Auto",   "AUT-702", 100.0,  "🚗");
    crearTarjeta("R-105", "Pedro Mamani (Camión)",  "Camion", "CAM-901", 2000.0, "🚚");

    int fila = 0;
    int col = 0;
    for (auto it = m_vehiculos.begin(); it != m_vehiculos.end(); ++it) {
        grid->addWidget(it.value().grupo, fila, col);
        col++;
        if (col >= 2) {
            col = 0;
            fila++;
        }
    }

    container->setLayout(grid);
    scroll->setWidget(container);
    mainLayout->addWidget(scroll);
}

void FlotaView::crearTarjeta(const QString& idRep,
                             const QString& nombre,
                             const QString& tipo,
                             const QString& placa,
                             double capKg,
                             const QString& icono) {
    auto grp = new QGroupBox(QString("%1 %2 — Placa: %3").arg(icono, nombre, placa), this);
    grp->setStyleSheet("QGroupBox { font-weight: bold; font-size: 13px; border: 1px solid #CFD8DC; border-radius: 6px; margin-top: 10px; padding: 10px; background: white; }"
                       "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; color: #37474F; }");

    auto lyt = new QVBoxLayout(grp);
    lyt->setSpacing(6);

    auto topInfo = new QHBoxLayout();
    auto lblCap = new QLabel(QString("📦 Capacidad: %1 kg").arg(capKg, 0, 'f', 0), grp);
    lblCap->setStyleSheet("color: #546E7A; font-size: 11px;");
    topInfo->addWidget(lblCap);
    topInfo->addStretch();

    auto lblEstado = new QLabel("🟢 DISPONIBLE", grp);
    lblEstado->setStyleSheet("background: #E8F5E9; color: #2E7D32; font-weight: bold; padding: 3px 8px; border-radius: 4px; font-size: 11px;");
    topInfo->addWidget(lblEstado);
    lyt->addLayout(topInfo);

    auto lblPedido = new QLabel("Sin asignación actual (En base)", grp);
    lblPedido->setStyleSheet("color: #455A64; font-size: 12px; font-style: italic;");
    lyt->addWidget(lblPedido);

    auto barra = new QProgressBar(grp);
    barra->setRange(0, 100);
    barra->setValue(0);
    barra->setTextVisible(true);
    barra->setStyleSheet("QProgressBar { border: 1px solid #B0BEC5; border-radius: 4px; text-align: center; height: 16px; font-size: 10px; }"
                         "QProgressBar::chunk { background-color: #0288D1; border-radius: 3px; }");
    lyt->addWidget(barra);

    auto lblEntregas = new QLabel("Entregas completadas: 0", grp);
    lblEntregas->setStyleSheet("color: #78909C; font-size: 11px;");
    lyt->addWidget(lblEntregas);

    TarjetaVehiculo tv;
    tv.idRepartidor = idRep;
    tv.nombreRepartidor = nombre;
    tv.tipoVehiculo = tipo;
    tv.placa = placa;
    tv.capacidadKg = capKg;
    tv.grupo = grp;
    tv.lblEstado = lblEstado;
    tv.lblPedido = lblPedido;
    tv.lblEntregas = lblEntregas;
    tv.barraProgreso = barra;
    tv.pedidosCompletados = 0;
    tv.enRuta = false;

    m_vehiculos[idRep] = tv;
}

void FlotaView::slotPedidoEnCamino(const QString& idPedido,
                                   const QString& origen,
                                   const QString& destino,
                                   const QString& repartidor,
                                   const QString& tipoVehiculo) {
    Q_UNUSED(tipoVehiculo);
    QString repLimpio = repartidor.trimmed();
    if (repLimpio.isEmpty()) return;

    // Búsqueda del repartidor por ID exacto o coincidencia de nombre
    for (auto it = m_vehiculos.begin(); it != m_vehiculos.end(); ++it) {
        if (it.key().compare(repLimpio, Qt::CaseInsensitive) == 0 ||
            it.value().nombreRepartidor.contains(repLimpio, Qt::CaseInsensitive) ||
            repLimpio.contains(it.key(), Qt::CaseInsensitive)) {
            it->enRuta = true;
            it->lblEstado->setText("🟠 EN RUTA");
            it->lblEstado->setStyleSheet("background: #FFF3E0; color: #E65100; font-weight: bold; padding: 3px 8px; border-radius: 4px; font-size: 11px;");
            it->lblPedido->setText(QString("📍 %1: %2 ➔ %3").arg(idPedido, origen, destino));
            it->lblPedido->setStyleSheet("color: #E65100; font-size: 12px; font-weight: bold;");
            it->barraProgreso->setValue(5);
            break;
        }
    }
}

void FlotaView::slotPedidoCompletado(const QString& idPedido, const QString& repartidor) {
    Q_UNUSED(idPedido);
    QString repLimpio = repartidor.trimmed();
    if (repLimpio.isEmpty()) return;

    for (auto it = m_vehiculos.begin(); it != m_vehiculos.end(); ++it) {
        if (it.key().compare(repLimpio, Qt::CaseInsensitive) == 0 ||
            it.value().nombreRepartidor.contains(repLimpio, Qt::CaseInsensitive) ||
            repLimpio.contains(it.key(), Qt::CaseInsensitive)) {
            it->enRuta = false;
            it->pedidosCompletados++;
            it->lblEstado->setText("🟢 DISPONIBLE");
            it->lblEstado->setStyleSheet("background: #E8F5E9; color: #2E7D32; font-weight: bold; padding: 3px 8px; border-radius: 4px; font-size: 11px;");
            it->lblPedido->setText("En base (Listo para nuevo despacho)");
            it->lblPedido->setStyleSheet("color: #455A64; font-size: 12px; font-style: italic;");
            it->barraProgreso->setValue(100);
            it->lblEntregas->setText(QString("Entregas completadas: %1").arg(it->pedidosCompletados));
            break;
        }
    }
}

void FlotaView::onTimerAnimacionProgreso() {
    for (auto it = m_vehiculos.begin(); it != m_vehiculos.end(); ++it) {
        if (it->enRuta) {
            int val = it->barraProgreso->value();
            if (val < 95) {
                it->barraProgreso->setValue(val + 2);
            }
        }
    }
}

void FlotaView::slotResetearFlota() {
    for (auto it = m_vehiculos.begin(); it != m_vehiculos.end(); ++it) {
        it->enRuta = false;
        it->pedidosCompletados = 0;
        it->lblEstado->setText("🟢 DISPONIBLE");
        it->lblEstado->setStyleSheet("background: #E8F5E9; color: #2E7D32; font-weight: bold; padding: 3px 8px; border-radius: 4px; font-size: 11px;");
        it->lblPedido->setText("Sin asignación actual (En base)");
        it->lblPedido->setStyleSheet("color: #455A64; font-size: 12px; font-style: italic;");
        it->barraProgreso->setValue(0);
        it->lblEntregas->setText("Entregas completadas: 0");
    }
}

} // namespace siger
