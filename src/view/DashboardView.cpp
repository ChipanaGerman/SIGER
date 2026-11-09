/**
 * @file DashboardView.cpp
 * @brief Implementación de la tabla de monitoreo de pedidos con filtros en tiempo real y exportación CSV.
 */

#include "DashboardView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>

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

    // Barra superior con conteo, buscador en vivo y botones de acción
    auto topLayout = new QHBoxLayout();
    m_lblConteo = new QLabel("Pedidos registrados: 0", this);
    m_lblConteo->setStyleSheet("font-weight: bold; font-size: 13px; color: #1A73E8;");
    topLayout->addWidget(m_lblConteo);

    m_txtBuscar = new QLineEdit(this);
    m_txtBuscar->setPlaceholderText("🔍 Filtrar pedidos por ID, calle, estado, vehículo...");
    m_txtBuscar->setClearButtonEnabled(true);
    m_txtBuscar->setStyleSheet("padding: 5px 10px; border: 1px solid #CFD8DC; border-radius: 4px; font-size: 12px;");
    connect(m_txtBuscar, &QLineEdit::textChanged, this, &DashboardView::onFiltrarTabla);
    topLayout->addWidget(m_txtBuscar, 1);

    auto btnExportar = new QPushButton("📥 Exportar CSV", this);
    btnExportar->setToolTip("Descargar reporte de auditoría en formato Excel / CSV");
    btnExportar->setStyleSheet("background-color: #2E7D32; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    connect(btnExportar, &QPushButton::clicked, this, &DashboardView::slotExportarCsv);
    topLayout->addWidget(btnExportar);

    auto btnLimpiar = new QPushButton("🗑 Limpiar Vista", this);
    btnLimpiar->setStyleSheet("background-color: white; border: 1px solid #B0BEC5; color: #37474F; font-weight: bold; padding: 6px 10px; border-radius: 4px;");
    connect(btnLimpiar, &QPushButton::clicked, this, &DashboardView::slotLimpiarTabla);
    topLayout->addWidget(btnLimpiar);

    layout->addLayout(topLayout);

    // Tabla con 10 columnas completas (incluyendo distancia y costo en Soles)
    m_tabla = new QTableWidget(this);
    m_tabla->setColumnCount(10);
    m_tabla->setHorizontalHeaderLabels({
        "ID Pedido", "Tipo", "Vehículo Req.", "Peso", "Origen", "Destino", "Distancia", "Costo", "Estado", "Acción"
    });
    m_tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tabla->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    m_tabla->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
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
        colorFondo = QColor(227, 242, 253); // Azul claro para nuevo
    }

    for (int col = 0; col < m_tabla->columnCount() - 1; ++col) {
        if (auto item = m_tabla->item(fila, col)) {
            item->setBackground(colorFondo);
        }
    }
}

void DashboardView::slotAgregarFila(const QString& id,
                                    const QString& tipo,
                                    const QString& estado,
                                    double peso,
                                    const QString& origen,
                                    const QString& destino,
                                    const QString& vehiculo,
                                    double costo,
                                    double distancia) {
    // Verificar si ya existe para actualizar en vez de duplicar
    for (int row = 0; row < m_tabla->rowCount(); ++row) {
        if (m_tabla->item(row, 0) && m_tabla->item(row, 0)->text() == id) {
            slotActualizarEstado(id, estado);
            return;
        }
    }

    int fila = m_tabla->rowCount();
    m_tabla->insertRow(fila);

    auto itemCentrado = [](const QString& txt) {
        auto item = new QTableWidgetItem(txt);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    m_tabla->setItem(fila, 0, itemCentrado(id));
    m_tabla->setItem(fila, 1, itemCentrado(tipo));
    m_tabla->setItem(fila, 2, itemCentrado(vehiculo));
    m_tabla->setItem(fila, 3, itemCentrado(QString("%1 kg").arg(peso, 0, 'f', 1)));
    m_tabla->setItem(fila, 4, new QTableWidgetItem(origen));
    m_tabla->setItem(fila, 5, new QTableWidgetItem(destino));
    m_tabla->setItem(fila, 6, itemCentrado(distancia > 0.0 ? QString("%1 km").arg(distancia, 0, 'f', 1) : "-"));
    
    // Costo en Soles resaltado
    auto itemCosto = itemCentrado(QString("S/. %1").arg(costo, 0, 'f', 2));
    itemCosto->setFont(QFont("Segoe UI", 9, QFont::Bold));
    m_tabla->setItem(fila, 7, itemCosto);

    m_tabla->setItem(fila, 8, itemCentrado(estado));

    auto btnCancelar = new QPushButton("Cancelar", this);
    btnCancelar->setStyleSheet("background-color: #EF5350; color: white; border-radius: 3px; padding: 2px 8px;");
    btnCancelar->setProperty("pedidoId", id);
    connect(btnCancelar, &QPushButton::clicked, this, &DashboardView::onBtnCancelarClicked);
    m_tabla->setCellWidget(fila, 9, btnCancelar);

    colorearFila(fila, estado);
    m_lblConteo->setText(QString("Pedidos registrados: %1").arg(m_tabla->rowCount()));

    // Aplicar filtro si el buscador tiene texto activo
    if (!m_txtBuscar->text().isEmpty()) {
        onFiltrarTabla(m_txtBuscar->text());
    }
}

void DashboardView::slotActualizarEstado(const QString& id, const QString& nuevoEstado) {
    for (int row = 0; row < m_tabla->rowCount(); ++row) {
        auto item = m_tabla->item(row, 0);
        if (item && item->text() == id) {
            if (auto itemEstado = m_tabla->item(row, 8)) {
                itemEstado->setText(nuevoEstado);
            }
            colorearFila(row, nuevoEstado);

            // Deshabilitar botón de cancelar si ya finalizó
            if (nuevoEstado.contains("COMPLETADO", Qt::CaseInsensitive) ||
                nuevoEstado.contains("CANCELADO", Qt::CaseInsensitive) ||
                nuevoEstado.contains("FALLIDO", Qt::CaseInsensitive)) {
                if (auto btn = qobject_cast<QPushButton*>(m_tabla->cellWidget(row, 9))) {
                    btn->setEnabled(false);
                    btn->setStyleSheet("background-color: #B0BEC5; color: white; border-radius: 3px; padding: 2px 8px;");
                }
            }
            break;
        }
    }

    if (!m_txtBuscar->text().isEmpty()) {
        onFiltrarTabla(m_txtBuscar->text());
    }
}

void DashboardView::onBtnCancelarClicked() {
    auto btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString id = btn->property("pedidoId").toString();
    if (!id.isEmpty()) {
        emit sigCancelarPedido(id);
    }
}

void DashboardView::slotLimpiarTabla() {
    m_tabla->setRowCount(0);
    m_lblConteo->setText("Pedidos registrados: 0");
}

void DashboardView::onFiltrarTabla(const QString& texto) {
    QString filtro = texto.trimmed().toLower();
    for (int r = 0; r < m_tabla->rowCount(); ++r) {
        if (filtro.isEmpty()) {
            m_tabla->setRowHidden(r, false);
            continue;
        }
        bool coincide = false;
        for (int c = 0; c < m_tabla->columnCount() - 1; ++c) {
            auto item = m_tabla->item(r, c);
            if (item && item->text().toLower().contains(filtro)) {
                coincide = true;
                break;
            }
        }
        m_tabla->setRowHidden(r, !coincide);
    }
}

void DashboardView::slotExportarCsv() {
    if (m_tabla->rowCount() == 0) {
        QMessageBox::warning(this, "Exportar Reporte", "No hay pedidos en la tabla para exportar.");
        return;
    }

    QString defaultName = QString("SIGER_Reporte_Pedidos_%1.csv")
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

    QString rutaArchivo = QFileDialog::getSaveFileName(
        this,
        "Guardar Reporte de Entregas (CSV)",
        defaultName,
        "Archivos CSV (*.csv);;Todos los archivos (*.*)"
    );

    if (rutaArchivo.isEmpty()) {
        return;
    }

    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error de Archivo", "No se pudo crear el archivo para escritura.");
        return;
    }

    QTextStream out(&archivo);
    // BOM UTF-8 para visualización perfecta en Microsoft Excel
    out.setGenerateByteOrderMark(true);

    // Encabezados CSV
    out << "ID Pedido,Tipo,Vehiculo Requerido,Peso (kg),Origen,Destino,Distancia (km),Costo (S/.),Estado\n";

    for (int r = 0; r < m_tabla->rowCount(); ++r) {
        QStringList filaValores;
        for (int c = 0; c < m_tabla->columnCount() - 1; ++c) {
            auto item = m_tabla->item(r, c);
            QString texto = item ? item->text() : "";
            // Escapar comas o comillas si las hubiera
            if (texto.contains(',') || texto.contains('"')) {
                texto = "\"" + texto.replace('"', "\"\"") + "\"";
            }
            filaValores.append(texto);
        }
        out << filaValores.join(",") << "\n";
    }

    archivo.close();
    QMessageBox::information(this, "Exportación Exitosa",
                             QString("El reporte se guardó correctamente con %1 pedidos registrados:\n%2")
                                 .arg(m_tabla->rowCount())
                                 .arg(rutaArchivo));
}

} // namespace siger
