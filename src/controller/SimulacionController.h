#pragma once
/**
 * @file SimulacionController.h
 * @brief Controlador MVC para el simulador multihilo de entregas.
 */

#include <QObject>
#include <QString>
#include <memory>

#include "SimuladorEntregas.h"
#include "GeneradorEventos.h"
#include "Observer.h"

namespace siger {

class SimulacionController : public QObject, public IObserver {
    Q_OBJECT

public:
    SimulacionController(std::shared_ptr<SimuladorEntregas> simulador,
                         std::shared_ptr<GeneradorEventos> generador,
                         QObject* parent = nullptr);
    ~SimulacionController() override = default;

    void onEvento(const DatoEvento& evento) override;

    [[nodiscard]] std::shared_ptr<SimuladorEntregas> getSimulador() const { return m_simulador; }

public slots:
    void slotIniciar();
    void slotPausar();
    void slotReanudar();
    void slotDetener();
    void slotSetVelocidad(int ms);

signals:
    void simulacionIniciada();
    void simulacionPausada();
    void simulacionDetenida();
    void entregaCompletada(const QString& idPedido, const QString& idRepartidor);
    void estadoActualizado(int entregados, int fallidos, int enCamino);

private:
    std::shared_ptr<SimuladorEntregas> m_simulador;
    std::shared_ptr<GeneradorEventos>  m_generador;
};

} // namespace siger
