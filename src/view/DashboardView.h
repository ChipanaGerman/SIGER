#pragma once
/**
 * @file DashboardView.h
 * @brief Tabla interactiva de monitoreo de pedidos con filtros en tiempo real y exportación CSV.
 */

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

namespace siger {

class DashboardView : public QWidget {
    Q_OBJECT

public:
    explicit DashboardView(QWidget* parent = nullptr);
    ~DashboardView() override = default;

public slots:
    void slotAgregarFila(const QString& id,
                         const QString& tipo,
                         const QString& estado,
                         double peso,
                         const QString& origen,
                         const QString& destino,
                         const QString& vehiculo = "Auto",
                         double costo = 0.0,
                         double distancia = 0.0);

    void slotActualizarEstado(const QString& id, const QString& nuevoEstado);
    void slotLimpiarTabla();
    void slotExportarCsv();

signals:
    void sigCancelarPedido(const QString& id);

private slots:
    void onBtnCancelarClicked();
    void onFiltrarTabla(const QString& texto);

private:
    void setupUi();
    void colorearFila(int fila, const QString& estado);

    QTableWidget* m_tabla{nullptr};
    QLabel*       m_lblConteo{nullptr};
    QLineEdit*    m_txtBuscar{nullptr};
};

} // namespace siger
