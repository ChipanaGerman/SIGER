/**
 * @file FormPedido.cpp
 * @brief Implementación del formulario de registro de pedidos.
 */

#include "FormPedido.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

namespace siger {

FormPedido::FormPedido(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void FormPedido::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    auto grpForm = new QGroupBox("Nuevo Pedido de Entrega", this);
    auto formLayout = new QFormLayout(grpForm);
    formLayout->setSpacing(10);

    m_txtDescripcion = new QLineEdit(this);
    m_txtDescripcion->setPlaceholderText("Ej: Documentos urgentes, laptop, etc.");
    formLayout->addRow("Descripción:", m_txtDescripcion);

    m_cmbTipo = new QComboBox(this);
    m_cmbTipo->addItem("Estándar");
    m_cmbTipo->addItem("Frágil (Cuidado especial)");
    formLayout->addRow("Tipo de Paquete:", m_cmbTipo);

    m_spnPeso = new QDoubleSpinBox(this);
    m_spnPeso->setRange(0.1, 2000.0);
    m_spnPeso->setValue(3.5);
    m_spnPeso->setSuffix(" kg");
    formLayout->addRow("Peso:", m_spnPeso);

    const QStringList ubicaciones = {
        "Plaza de Armas",
        "Av. Ejercito 450",
        "Calle Mercaderes 120",
        "Av. Dolores 890",
        "Urb. Umacollo B-12",
        "Av. Cayma 310",
        "Parque Industrial",
        "Av. Lambramani 105",
        "Calle San Francisco 204",
        "Av. Venezuela 800 (UNSA)"
    };

    m_cmbOrigen = new QComboBox(this);
    m_cmbOrigen->addItems(ubicaciones);
    m_cmbOrigen->setCurrentIndex(0);
    formLayout->addRow("Punto de Origen:", m_cmbOrigen);

    m_cmbDestino = new QComboBox(this);
    m_cmbDestino->addItems(ubicaciones);
    m_cmbDestino->setCurrentIndex(3);
    formLayout->addRow("Punto de Destino:", m_cmbDestino);

    m_spnDistancia = new QDoubleSpinBox(this);
    m_spnDistancia->setRange(0.5, 100.0);
    m_spnDistancia->setValue(6.8);
    m_spnDistancia->setSuffix(" km");
    formLayout->addRow("Distancia Est.:", m_spnDistancia);

    m_cmbVehiculo = new QComboBox(this);
    m_cmbVehiculo->addItem("Moto (hasta 15kg)");
    m_cmbVehiculo->addItem("Auto (hasta 100kg)");
    m_cmbVehiculo->addItem("Camión (hasta 2000kg)");
    formLayout->addRow("Vehículo Sugerido:", m_cmbVehiculo);

    // Ajustar vehículo sugerido según el peso automáticamente
    connect(m_spnPeso, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (val <= 15.0) {
            m_cmbVehiculo->setCurrentIndex(0); // Moto
        } else if (val <= 100.0) {
            m_cmbVehiculo->setCurrentIndex(1); // Auto
        } else {
            m_cmbVehiculo->setCurrentIndex(2); // Camión
        }
    });

    m_btnRegistrar = new QPushButton("➕ Registrar Pedido", this);
    m_btnRegistrar->setStyleSheet("background-color: #0288D1; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    formLayout->addRow(m_btnRegistrar);

    mainLayout->addWidget(grpForm);
    mainLayout->addStretch();

    connect(m_btnRegistrar, &QPushButton::clicked, this, &FormPedido::onBtnRegistrarClicked);
}

void FormPedido::onBtnRegistrarClicked() {
    QString desc = m_txtDescripcion->text().trimmed();
    if (desc.isEmpty()) {
        desc = "Paquete de envío estándar";
    }

    QString tipo = m_cmbTipo->currentText();
    double peso = m_spnPeso->value();
    QString origen = m_cmbOrigen->currentText();
    QString destino = m_cmbDestino->currentText();
    double dist = m_spnDistancia->value();
    QString vehiculo = m_cmbVehiculo->currentText();

    emit sigNuevoPedido(tipo, desc, peso, origen, destino, dist, vehiculo);

    m_txtDescripcion->clear();
    m_spnPeso->setValue(2.0);
}

} // namespace siger
