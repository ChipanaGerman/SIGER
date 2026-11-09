/**
 * @file FormPedido.cpp
 * @brief Implementación del formulario de registro de pedidos.
 */

#include "FormPedido.h"
#include "ServicioRutas.h"
#include "IEstrategiaTarifa.h"
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
        "Calle Mercaderes 120",
        "Calle San Francisco 204",
        "Mirador de Yanahuara",
        "Av. Ejercito 450",
        "Av. Cayma 310",
        "Urb. Umacollo B-12",
        "UNSA Ingenierias (Av. Independencia)",
        "Av. Venezuela 800 (UNSA)",
        "Av. Lambramani 105",
        "Mall Aventura Porongoche",
        "Av. Dolores 890",
        "Plaza Mayta Capac (Miraflores)",
        "Parque Selva Alegre",
        "Mirador de Sachaca",
        "Terminal Terrestre Arequipa",
        "Cerro Colorado (Plaza Las Americas)",
        "Parque Industrial",
        "Plaza de Paucarpata",
        "Molino de Sabandia",
        "Monasterio de Santa Catalina",
        "Mercado San Camilo",
        "Hospital Regional Honorio Delgado",
        "Hospital Goyeneche",
        "Estadio Melgar (IV Centenario)",
        "Real Plaza Arequipa (Cayma)",
        "Plaza de Cayma (San Miguel)",
        "Plataforma Andres Avelino Caceres",
        "Balneario de Tingo",
        "Urb. Vallecito"
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
    m_cmbVehiculo->addItem("Auto-asignación inteligente (Cualquier vehículo disponible)");
    m_cmbVehiculo->addItem("Moto exclusiva (hasta 15kg)");
    m_cmbVehiculo->addItem("Auto exclusivo (hasta 100kg)");
    m_cmbVehiculo->addItem("Camión exclusivo (hasta 2000kg)");
    formLayout->addRow("Vehículo Sugerido:", m_cmbVehiculo);

    m_lblCosto = new QLabel("S/. 0.00", this);
    m_lblCosto->setStyleSheet("font-weight: bold; font-size: 14px; color: #2E7D32; background: #E8F5E9; padding: 4px 8px; border-radius: 4px;");
    formLayout->addRow("Tarifa Estimada:", m_lblCosto);

    // Ajustar vehículo sugerido según el peso automáticamente (si no está en auto-asignación)
    connect(m_spnPeso, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (m_cmbVehiculo->currentIndex() != 0) {
            if (val <= 15.0) {
                m_cmbVehiculo->setCurrentIndex(1); // Moto
            } else if (val <= 100.0) {
                m_cmbVehiculo->setCurrentIndex(2); // Auto
            } else {
                m_cmbVehiculo->setCurrentIndex(3); // Camión
            }
        }
        actualizarEstimaciones();
    });

    connect(m_cmbOrigen, &QComboBox::currentTextChanged, this, &FormPedido::actualizarEstimaciones);
    connect(m_cmbDestino, &QComboBox::currentTextChanged, this, &FormPedido::actualizarEstimaciones);
    connect(m_cmbTipo, &QComboBox::currentTextChanged, this, &FormPedido::actualizarEstimaciones);
    connect(m_spnDistancia, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
        // Recalcular costo si el usuario altera manualmente la distancia
        bool esFragil = m_cmbTipo->currentText().contains("Frágil", Qt::CaseInsensitive);
        double dist = m_spnDistancia->value();
        double peso = m_spnPeso->value();
        double costo = 0.0;
        if (peso > 100.0) {
            costo = TarifaPesadaStrategy().calcularTarifa(dist, peso, esFragil);
        } else if (esFragil) {
            costo = TarifaFragilStrategy().calcularTarifa(dist, peso, esFragil);
        } else {
            costo = TarifaEstandarStrategy().calcularTarifa(dist, peso, esFragil);
        }
        m_lblCosto->setText(QString("S/. %1").arg(costo, 0, 'f', 2));
    });

    actualizarEstimaciones();

    m_btnRegistrar = new QPushButton("➕ Registrar Pedido", this);
    m_btnRegistrar->setStyleSheet("background-color: #0288D1; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    formLayout->addRow(m_btnRegistrar);

    mainLayout->addWidget(grpForm);
    mainLayout->addStretch();

    connect(m_btnRegistrar, &QPushButton::clicked, this, &FormPedido::onBtnRegistrarClicked);
}

void FormPedido::actualizarEstimaciones() {
    QString o = m_cmbOrigen->currentText();
    QString d = m_cmbDestino->currentText();
    double km = ServicioRutas::instancia().calcularDistanciaKm(o, d);
    m_spnDistancia->setValue(km);

    double peso = m_spnPeso->value();
    bool esFragil = m_cmbTipo->currentText().contains("Frágil", Qt::CaseInsensitive);

    double costo = 0.0;
    if (peso > 100.0) {
        costo = TarifaPesadaStrategy().calcularTarifa(km, peso, esFragil);
    } else if (esFragil) {
        costo = TarifaFragilStrategy().calcularTarifa(km, peso, esFragil);
    } else {
        costo = TarifaEstandarStrategy().calcularTarifa(km, peso, esFragil);
    }
    m_lblCosto->setText(QString("S/. %1").arg(costo, 0, 'f', 2));
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

    bool esFragil = tipo.contains("Frágil", Qt::CaseInsensitive);
    double costo = 0.0;
    if (peso > 100.0) {
        costo = TarifaPesadaStrategy().calcularTarifa(dist, peso, esFragil);
    } else if (esFragil) {
        costo = TarifaFragilStrategy().calcularTarifa(dist, peso, esFragil);
    } else {
        costo = TarifaEstandarStrategy().calcularTarifa(dist, peso, esFragil);
    }

    emit sigNuevoPedido(tipo, desc, peso, origen, destino, dist, vehiculo, costo);

    m_txtDescripcion->clear();
    m_spnPeso->setValue(2.0);
    m_cmbVehiculo->setCurrentIndex(0); // Volver a auto-asignación por defecto
}

} // namespace siger
