#pragma once
/**
 * @file MainWindow.h
 * @brief Ventana principal de la aplicación SIGER.
 */

#include <QMainWindow>
#include <QTabWidget>
#include <QDockWidget>
#include <memory>

#include "PedidoController.h"
#include "SimulacionController.h"
#include "DashboardView.h"
#include "MapaView.h"
#include "EstadisticasView.h"
#include "ControlPanel.h"
#include "FormPedido.h"

namespace siger {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<PedidoController> pedidoCtrl,
               std::shared_ptr<SimulacionController> simulacionCtrl,
               QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAcercaDe();

private:
    void setupUi();
    void conectarSignalsSlots();

    std::shared_ptr<PedidoController>     m_pedidoCtrl;
    std::shared_ptr<SimulacionController> m_simulacionCtrl;

    QTabWidget*       m_tabWidget{nullptr};
    DashboardView*    m_dashboardView{nullptr};
    MapaView*         m_mapaView{nullptr};
    EstadisticasView* m_estadisticasView{nullptr};
    ControlPanel*     m_controlPanel{nullptr};
    FormPedido*       m_formPedido{nullptr};
};

} // namespace siger
