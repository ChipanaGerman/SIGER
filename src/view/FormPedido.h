#pragma once
/**
 * @file FormPedido.h
 * @brief Formulario interactivo para registrar nuevos pedidos de entrega en Qt.
 */

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>

namespace siger {

class FormPedido : public QWidget {
    Q_OBJECT

public:
    explicit FormPedido(QWidget* parent = nullptr);
    ~FormPedido() override = default;

signals:
    void sigNuevoPedido(const QString& tipo,
                        const QString& descripcion,
                        double peso,
                        const QString& origen,
                        const QString& destino,
                        double distancia,
                        const QString& tipoVehiculo);

private slots:
    void onBtnRegistrarClicked();

private:
    void setupUi();

    QLineEdit*      m_txtDescripcion{nullptr};
    QComboBox*      m_cmbTipo{nullptr};
    QDoubleSpinBox* m_spnPeso{nullptr};
    QComboBox*      m_cmbOrigen{nullptr};
    QComboBox*      m_cmbDestino{nullptr};
    QDoubleSpinBox* m_spnDistancia{nullptr};
    QComboBox*      m_cmbVehiculo{nullptr};
    QPushButton*    m_btnRegistrar{nullptr};
};

} // namespace siger
