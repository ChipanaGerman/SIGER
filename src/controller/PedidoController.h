#pragma once
/**
 * @file PedidoController.h
 * @brief Controlador MVC para la gestión de pedidos en SIGER.
 *
 * Conecta las vistas de la GUI con el modelo GestorPedidos e implementa
 * IObserver para responder a cambios de estado del modelo.
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
                           const QString& tipoVehiculo);

    void slotCancelarPedido(const QString& id);
    void slotRefrescarTodos();

signals:
    void pedidoAgregado(const QString& id,
                        const QString& tipo,
                        const QString& estado,
                        double peso,
                        const QString& origen,
                        const QString& destino);

    void pedidoActualizado(const QString& id, const QString& nuevoEstado);
    void estadisticasActualizadas(int total, int pendientes, int completados, int cancelados);

private:
    std::shared_ptr<GestorPedidos> m_gestor;
    std::atomic<int> m_contadorId{500};
};

} // namespace siger
