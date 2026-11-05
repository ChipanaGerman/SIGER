#pragma once
/**
 * @file DashboardView.h
 * @brief Tabla de monitoreo en tiempo real de pedidos de entrega.
 */

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

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
                         const QString& destino);

    void slotActualizarEstado(const QString& id, const QString& nuevoEstado);
    void slotLimpiarTabla();

signals:
    void sigCancelarPedido(const QString& id);

private slots:
    void onBtnCancelarClicked();

private:
    void setupUi();
    void colorearFila(int fila, const QString& estado);

    QTableWidget* m_tabla{nullptr};
    QLabel*       m_lblConteo{nullptr};
};

} // namespace siger
