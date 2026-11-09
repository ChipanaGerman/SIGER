#pragma once
/**
 * @file PedidoController.h
 * @brief Controlador MVC para la gestión de pedidos en SIGER.
 */

#include <QObject>
#include <QString>
#include <memory>

#include "GestorPedidos.h"
#include "Observer.h"

namespace siger {

class PedidoController : public QObject, public IObserver {
    Q_OBJECT

public:
    explicit PedidoController(std::shared_ptr<GestorPedidos> gestor, QObject* parent = nullptr);
    ~PedidoController() override = default;

    // Implementación de IObserver
    void onEvento(const DatoEvento& evento) override;

    [[nodiscard]] std::shared_ptr<GestorPedidos> getGestor() const { return m_gestor; }

public slots:
    void slotAgregarPedido(const QString& tipo,
                           const QString& descripcion,
                           double peso,
                           const QString& origen,
                           const QString& destino,
                           double distancia,
                           const QString& tipoVehiculo,
                           double costo = 0.0);

    void slotCancelarPedido(const QString& id);
    void slotRefrescarTodos();

signals:
    void pedidoAgregado(const QString& id,
                        const QString& tipo,
                        const QString& estado,
                        double peso,
                        const QString& origen,
                        const QString& destino,
                        const QString& vehiculo = "Auto",
                        double costo = 0.0,
                        double distancia = 0.0);

    void pedidoActualizado(const QString& id, const QString& nuevoEstado);
    void pedidoEnCamino(const QString& id,
                        const QString& origen,
                        const QString& destino,
                        const QString& repartidor,
                        const QString& tipoVehiculo = "Auto");
    void estadisticasActualizadas(int total, int pendientes, int completados, int cancelados, double totalFacturado = 0.0);

private:
    std::shared_ptr<GestorPedidos> m_gestor;
    std::atomic<int> m_contadorId{500};
};

} // namespace siger
