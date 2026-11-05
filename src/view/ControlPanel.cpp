/**
 * @file ControlPanel.cpp
 * @brief Implementación del panel de control de simulación.
 */

#include "ControlPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

namespace siger {

ControlPanel::ControlPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void ControlPanel::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    auto grpControl = new QGroupBox("Control de Simulación", this);
    auto grpLayout = new QVBoxLayout(grpControl);

    // Estado actual
    m_lblEstado = new QLabel("Estado: DETENIDO", this);
    m_lblEstado->setStyleSheet("font-weight: bold; font-size: 13px; color: #757575; padding: 6px; background: #EEEEEE; border-radius: 4px;");
    m_lblEstado->setAlignment(Qt::AlignCenter);
    grpLayout->addWidget(m_lblEstado);

    // Botones
    m_btnIniciar = new QPushButton("▶ Iniciar Simulación", this);
    m_btnIniciar->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");

    m_btnPausar = new QPushButton("⏸ Pausar", this);
    m_btnPausar->setStyleSheet("background-color: #FF9800; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    m_btnPausar->setEnabled(false);

    m_btnDetener = new QPushButton("⏹ Detener", this);
    m_btnDetener->setStyleSheet("background-color: #F44336; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    m_btnDetener->setEnabled(false);

    grpLayout->addWidget(m_btnIniciar);
    grpLayout->addWidget(m_btnPausar);
    grpLayout->addWidget(m_btnDetener);

    // Slider de velocidad
    grpLayout->addSpacing(10);
    m_lblVelocidad = new QLabel("Velocidad: 1000 ms/paso", this);
    grpLayout->addWidget(m_lblVelocidad);

    m_sliderVelocidad = new QSlider(Qt::Horizontal, this);
    m_sliderVelocidad->setRange(100, 3000);
    m_sliderVelocidad->setValue(1000);
    m_sliderVelocidad->setSingleStep(100);
    grpLayout->addWidget(m_sliderVelocidad);

    grpLayout->addStretch();
    mainLayout->addWidget(grpControl);

    // Conexiones
    connect(m_btnIniciar, &QPushButton::clicked, this, &ControlPanel::onBtnIniciarClicked);
    connect(m_btnPausar, &QPushButton::clicked, this, &ControlPanel::onBtnPausarClicked);
    connect(m_btnDetener, &QPushButton::clicked, this, &ControlPanel::onBtnDetenerClicked);
    connect(m_sliderVelocidad, &QSlider::valueChanged, this, &ControlPanel::onSliderVelocidadChanged);
}

void ControlPanel::setEstadoSimulacion(const QString& estado, const QString& colorHex) {
    m_lblEstado->setText("Estado: " + estado);
    m_lblEstado->setStyleSheet(QString("font-weight: bold; font-size: 13px; color: white; padding: 6px; background: %1; border-radius: 4px;").arg(colorHex));
}

void ControlPanel::onBtnIniciarClicked() {
    m_btnIniciar->setEnabled(false);
    m_btnPausar->setEnabled(true);
    m_btnDetener->setEnabled(true);
    m_btnPausar->setText("⏸ Pausar");
    m_enPausa = false;

    setEstadoSimulacion("EN EJECUCIÓN", "#4CAF50");
    emit sigIniciar();
}

void ControlPanel::onBtnPausarClicked() {
    if (!m_enPausa) {
        m_enPausa = true;
        m_btnPausar->setText("▶ Reanudar");
        setEstadoSimulacion("PAUSADO", "#FF9800");
        emit sigPausar();
    } else {
        m_enPausa = false;
        m_btnPausar->setText("⏸ Pausar");
        setEstadoSimulacion("EN EJECUCIÓN", "#4CAF50");
        emit sigReanudar();
    }
}

void ControlPanel::onBtnDetenerClicked() {
    m_btnIniciar->setEnabled(true);
    m_btnPausar->setEnabled(false);
    m_btnDetener->setEnabled(false);
    m_btnPausar->setText("⏸ Pausar");
    m_enPausa = false;

    setEstadoSimulacion("DETENIDO", "#F44336");
    emit sigDetener();
}

void ControlPanel::onSliderVelocidadChanged(int val) {
    m_lblVelocidad->setText(QString("Velocidad: %1 ms/paso").arg(val));
    emit sigVelocidadCambiada(val);
}

} // namespace siger
