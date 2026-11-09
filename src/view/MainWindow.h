#pragma once
/**
 * @file MainWindow.h
 * @brief Ventana principal de la aplicación SIGER con Centro de Control Unificado (Cockpit).
 */

#include <QMainWindow>
#include <QTabWidget>
#include <QDockWidget>
#include <QSplitter>
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
    void slotTogglePanelInferior();

private:
    void setupUi();
    void conectarSignalsSlots();

    std::shared_ptr<PedidoController>     m_pedidoCtrl;
    std::shared_ptr<SimulacionController> m_simulacionCtrl;

    QSplitter*        m_splitterPrincipal{nullptr};
    QTabWidget*       m_tabPanelInferior{nullptr};
    DashboardView*    m_dashboardView{nullptr};
    MapaView*         m_mapaView{nullptr};
    class FlotaView*  m_flotaView{nullptr};
    EstadisticasView* m_estadisticasView{nullptr};
    ControlPanel*     m_controlPanel{nullptr};
    FormPedido*       m_formPedido{nullptr};
};

} // namespace siger
