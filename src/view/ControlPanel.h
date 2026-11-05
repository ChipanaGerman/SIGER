#pragma once
/**
 * @file ControlPanel.h
 * @brief Panel de control de simulación en Qt.
 */

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

namespace siger {

class ControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel() override = default;

public slots:
    void setEstadoSimulacion(const QString& estado, const QString& colorHex = "#2196F3");

signals:
    void sigIniciar();
    void sigPausar();
    void sigReanudar();
    void sigDetener();
    void sigVelocidadCambiada(int ms);

private slots:
    void onBtnIniciarClicked();
    void onBtnPausarClicked();
    void onBtnDetenerClicked();
    void onSliderVelocidadChanged(int val);

private:
    void setupUi();

    QPushButton* m_btnIniciar{nullptr};
    QPushButton* m_btnPausar{nullptr};
    QPushButton* m_btnDetener{nullptr};
    QSlider*     m_sliderVelocidad{nullptr};
    QLabel*      m_lblVelocidad{nullptr};
    QLabel*      m_lblEstado{nullptr};
    bool         m_enPausa{false};
};

} // namespace siger
