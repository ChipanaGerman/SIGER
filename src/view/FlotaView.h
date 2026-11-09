#pragma once
/**
 * @file FlotaView.h
 * @brief Panel visual para monitoreo en tiempo real de la flota de repartidores y vehículos.
 */

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QMap>
#include <QTimer>

namespace siger {

struct TarjetaVehiculo {
    QString idRepartidor;
    QString nombreRepartidor;
    QString tipoVehiculo;
    QString placa;
    double capacidadKg;

    QGroupBox*    grupo{nullptr};
    QLabel*       lblEstado{nullptr};
    QLabel*       lblPedido{nullptr};
    QLabel*       lblEntregas{nullptr};
    QProgressBar* barraProgreso{nullptr};

    int pedidosCompletados{0};
    bool enRuta{false};
};

class FlotaView : public QWidget {
    Q_OBJECT

public:
    explicit FlotaView(QWidget* parent = nullptr);
    ~FlotaView() override = default;

public slots:
    void slotPedidoEnCamino(const QString& idPedido,
                            const QString& origen,
                            const QString& destino,
                            const QString& repartidor,
                            const QString& tipoVehiculo);

    void slotPedidoCompletado(const QString& idPedido, const QString& repartidor);
    void slotResetearFlota();

private slots:
    void onTimerAnimacionProgreso();

private:
    void setupUi();
    void crearTarjeta(const QString& idRep,
                      const QString& nombre,
                      const QString& tipo,
                      const QString& placa,
                      double capKg,
                      const QString& icono);

    QMap<QString, TarjetaVehiculo> m_vehiculos;
    QTimer* m_timerProgreso{nullptr};
};

} // namespace siger
