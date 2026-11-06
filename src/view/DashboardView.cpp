/**
 * @file DashboardView.cpp
 * @brief Implementación de la tabla de monitoreo de pedidos.
 */

#include "DashboardView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

namespace siger {

DashboardView::DashboardView(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void DashboardView::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    auto topLayout = new QHBoxLayout();
    m_lblConteo = new QLabel("Pedidos registrados: 0", this);
    m_lblConteo->setStyleSheet("font-weight: bold; font-size: 13px;");
    topLayout->addWidget(m_lblConteo);
    topLayout->addStretch();

    auto btnLimpiar = new QPushButton("🗑 Limpiar Vista", this);
    connect(btnLimpiar, &QPushButton::clicked, this, &DashboardView::slotLimpiarTabla);
    topLayout->addWidget(btnLimpiar);

    layout->addLayout(topLayout);

    m_tabla = new QTableWidget(this);
    m_tabla->setColumnCount(8);
    m_tabla->setHorizontalHeaderLabels({
        "ID Pedido", "Tipo", "Vehículo Req.", "Peso (kg)", "Origen", "Destino", "Estado", "Acción"
    });
    m_tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tabla->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    m_tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tabla->setAlternatingRowColors(true);

    layout->addWidget(m_tabla);
}

void DashboardView::colorearFila(int fila, const QString& estado) {
    QColor colorFondo;
    if (estado.contains("COMPLETADO", Qt::CaseInsensitive)) {
        colorFondo = QColor(200, 230, 201); // Verde suave
    } else if (estado.contains("CAMINO", Qt::CaseInsensitive) || estado.contains("TRANSITO", Qt::CaseInsensitive)) {
        colorFondo = QColor(255, 249, 196); // Amarillo suave
    } else if (estado.contains("CANCELADO", Qt::CaseInsensitive) || estado.contains("FALLIDO", Qt::CaseInsensitive)) {
        colorFondo = QColor(255, 205, 210); // Rojo suave
    } else {
        colorFondo = QColor(227, 242, 253); // Azul suave (NUEVO / ASIGNADO)
    }

    for (int col = 0; col < 7; ++col) {
        if (auto itm = m_tabla->item(fila, col)) {
            itm->setBackground(colorFondo);
        }
    }
}

void DashboardView::slotAgregarFila(const QString& id,
                                   const QString& tipo,
                                   const QString& estado,
                                   double peso,
                                   const QString& origen,
                                   const QString& destino,
                                   const QString& vehiculo) {
    // Verificar si ya existe
    for (int r = 0; r < m_tabla->rowCount(); ++r) {
        if (m_tabla->item(r, 0) && m_tabla->item(r, 0)->text() == id) {
            slotActualizarEstado(id, estado);
            return;
        }
    }

    int fila = m_tabla->rowCount();
    m_tabla->insertRow(fila);

    auto itmId = new QTableWidgetItem(id);
    itmId->setTextAlignment(Qt::AlignCenter);
    m_tabla->setItem(fila, 0, itmId);

    auto itmTipo = new QTableWidgetItem(tipo);
    itmTipo->setTextAlignment(Qt::AlignCenter);
    m_tabla->setItem(fila, 1, itmTipo);

    // Formatear icono de vehículo
    QString txtVehiculo = vehiculo.isEmpty() ? "Auto" : vehiculo;
    if (txtVehiculo.contains("Moto", Qt::CaseInsensitive)) {
        txtVehiculo = "🏍️ Moto";
    } else if (txtVehiculo.contains("Camion", Qt::CaseInsensitive) || txtVehiculo.contains("Camión", Qt::CaseInsensitive)) {
        txtVehiculo = "🚚 Camión";
    } else {
        txtVehiculo = "🚗 Auto";
    }
    auto itmVehiculo = new QTableWidgetItem(txtVehiculo);
    itmVehiculo->setTextAlignment(Qt::AlignCenter);
    m_tabla->setItem(fila, 2, itmVehiculo);

    auto itmPeso = new QTableWidgetItem(QString::number(peso, 'f', 1) + " kg");
    itmPeso->setTextAlignment(Qt::AlignCenter);
    m_tabla->setItem(fila, 3, itmPeso);

    m_tabla->setItem(fila, 4, new QTableWidgetItem(origen));
    m_tabla->setItem(fila, 5, new QTableWidgetItem(destino));

    auto itmEstado = new QTableWidgetItem(estado);
    itmEstado->setTextAlignment(Qt::AlignCenter);
    m_tabla->setItem(fila, 6, itmEstado);

    auto btnCancelar = new QPushButton("Cancelar", this);
    btnCancelar->setStyleSheet("background-color: #e57373; color: white; padding: 2px 6px; border-radius: 3px;");
    btnCancelar->setProperty("pedidoId", id);
    connect(btnCancelar, &QPushButton::clicked, this, &DashboardView::onBtnCancelarClicked);
    m_tabla->setCellWidget(fila, 7, btnCancelar);

    colorearFila(fila, estado);

    m_lblConteo->setText(QString("Pedidos registrados: %1").arg(m_tabla->rowCount()));
}

void DashboardView::slotActualizarEstado(const QString& id, const QString& nuevoEstado) {
    for (int r = 0; r < m_tabla->rowCount(); ++r) {
        if (m_tabla->item(r, 0) && m_tabla->item(r, 0)->text() == id) {
            if (auto itm = m_tabla->item(r, 6)) {
                itm->setText(nuevoEstado);
            }
            colorearFila(r, nuevoEstado);

            if (nuevoEstado.contains("COMPLETADO", Qt::CaseInsensitive) ||
                nuevoEstado.contains("CANCELADO", Qt::CaseInsensitive)) {
                if (auto btn = qobject_cast<QPushButton*>(m_tabla->cellWidget(r, 7))) {
                    btn->setEnabled(false);
                }
            }
            break;
        }
    }
}

void DashboardView::slotLimpiarTabla() {
    m_tabla->setRowCount(0);
    m_lblConteo->setText("Pedidos registrados: 0");
}

void DashboardView::onBtnCancelarClicked() {
    auto btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString id = btn->property("pedidoId").toString();
    emit sigCancelarPedido(id);
}

} // namespace siger
