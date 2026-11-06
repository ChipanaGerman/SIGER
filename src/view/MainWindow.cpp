/**
 * @file MainWindow.cpp
 * @brief Implementación de la ventana principal de SIGER.
 */

#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>

namespace siger {

MainWindow::MainWindow(std::shared_ptr<PedidoController> pedidoCtrl,
                       std::shared_ptr<SimulacionController> simulacionCtrl,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_pedidoCtrl(std::move(pedidoCtrl))
    , m_simulacionCtrl(std::move(simulacionCtrl))
{
    setupUi();
    conectarSignalsSlots();
}

void MainWindow::setupUi() {
    setWindowTitle("SIGER — Sistema Integrado de Gestión y Simulación de Entregas Rápidas");
    resize(1280, 820);

    // Widget central con pestañas
    m_tabWidget = new QTabWidget(this);
    m_dashboardView = new DashboardView(this);
    m_mapaView = new MapaView(this);
    m_estadisticasView = new EstadisticasView(this);

    m_tabWidget->addTab(m_dashboardView, "📋 Dashboard de Pedidos");
    m_tabWidget->addTab(m_mapaView, "🗺️ Mapa Interactivo");
    m_tabWidget->addTab(m_estadisticasView, "📊 Estadísticas y Gráficos");

    setCentralWidget(m_tabWidget);

    // Dock izquierdo: Panel de Control
    auto dockControl = new QDockWidget("Controles", this);
    dockControl->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_controlPanel = new ControlPanel(dockControl);
    dockControl->setWidget(m_controlPanel);
    addDockWidget(Qt::LeftDockWidgetArea, dockControl);

    // Dock derecho: Formulario de Registro
    auto dockForm = new QDockWidget("Registrar Entrega", this);
    dockForm->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_formPedido = new FormPedido(dockForm);
    dockForm->setWidget(m_formPedido);
    addDockWidget(Qt::RightDockWidgetArea, dockForm);

    // Menú superior
    auto menuArchivo = menuBar()->addMenu("&Archivo");
    auto actSalir = menuArchivo->addAction("Salir");
    connect(actSalir, &QAction::triggered, this, &QWidget::close);

    auto menuSimulacion = menuBar()->addMenu("&Simulación");
    auto actIniciar = menuSimulacion->addAction("Iniciar");
    auto actPausar = menuSimulacion->addAction("Pausar");
    auto actDetener = menuSimulacion->addAction("Detener");

    connect(actIniciar, &QAction::triggered, m_controlPanel, [this]() {
        if (m_simulacionCtrl) m_simulacionCtrl->slotIniciar();
    });
    connect(actPausar, &QAction::triggered, m_controlPanel, [this]() {
        if (m_simulacionCtrl) m_simulacionCtrl->slotPausar();
    });
    connect(actDetener, &QAction::triggered, m_controlPanel, [this]() {
        if (m_simulacionCtrl) m_simulacionCtrl->slotDetener();
    });

    auto menuAyuda = menuBar()->addMenu("A&yuda");
    auto actAcercaDe = menuAyuda->addAction("Acerca de SIGER");
    connect(actAcercaDe, &QAction::triggered, this, &MainWindow::onAcercaDe);

    // Barra de estado
    statusBar()->showMessage("SIGER Listo. Universidad Nacional de San Agustín (UNSA) - Tecnología de Objetos");
}

void MainWindow::conectarSignalsSlots() {
    if (!m_pedidoCtrl || !m_simulacionCtrl) return;

    // ── Conexiones de Pedidos ─────────────────────────────────────────
    connect(m_formPedido, &FormPedido::sigNuevoPedido,
            m_pedidoCtrl.get(), &PedidoController::slotAgregarPedido);

    connect(m_pedidoCtrl.get(), &PedidoController::pedidoAgregado,
            m_dashboardView, &DashboardView::slotAgregarFila);

    connect(m_pedidoCtrl.get(), &PedidoController::pedidoActualizado,
            m_dashboardView, &DashboardView::slotActualizarEstado);

    connect(m_dashboardView, &DashboardView::sigCancelarPedido,
            m_pedidoCtrl.get(), &PedidoController::slotCancelarPedido);

    connect(m_pedidoCtrl.get(), &PedidoController::estadisticasActualizadas,
            m_estadisticasView, &EstadisticasView::slotActualizarEstadisticas);

    // Animación en el mapa cuando un repartidor toma un pedido y se pone EN CAMINO
    connect(m_pedidoCtrl.get(), &PedidoController::pedidoEnCamino,
            this, [this](const QString& id, const QString& origen, const QString& destino, const QString& rep, const QString& vehiculo) {
                m_mapaView->animarEntrega(id, origen, destino, rep, vehiculo);
            });

    // ── Conexiones de Simulación ──────────────────────────────────────
    connect(m_controlPanel, &ControlPanel::sigIniciar,
            m_simulacionCtrl.get(), &SimulacionController::slotIniciar);

    connect(m_controlPanel, &ControlPanel::sigPausar,
            m_simulacionCtrl.get(), &SimulacionController::slotPausar);

    connect(m_controlPanel, &ControlPanel::sigReanudar,
            m_simulacionCtrl.get(), &SimulacionController::slotReanudar);

    connect(m_controlPanel, &ControlPanel::sigDetener,
            m_simulacionCtrl.get(), &SimulacionController::slotDetener);

    connect(m_controlPanel, &ControlPanel::sigVelocidadCambiada,
            m_simulacionCtrl.get(), &SimulacionController::slotSetVelocidad);

    connect(m_simulacionCtrl.get(), &SimulacionController::simulacionIniciada,
            this, [this]() {
                statusBar()->showMessage("Simulación en ejecución activa...");
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::simulacionDetenida,
            this, [this]() {
                statusBar()->showMessage("Simulación detenida.");
                m_mapaView->resetearMapa();
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::todosCompletados,
            this, [this]() {
                statusBar()->showMessage("✅ ¡Todos los pedidos completados! Simulación finalizada automáticamente.");
                QMessageBox::information(this, "Simulación completada",
                    "<b>¡Todas las entregas han sido completadas!</b><br>"
                    "La simulación se ha detenido automáticamente.<br><br>"
                    "Revisa las estadísticas para ver el resumen final.");
            });
}

void MainWindow::onAcercaDe() {
    QMessageBox::about(this, "Acerca de SIGER",
        "<h3>SIGER v1.0</h3>"
        "<p><b>Sistema Integrado de Gestión y Simulación de Entregas Rápidas</b></p>"
        "<p>Curso: <i>Tecnología de Objetos</i><br>"
        "Universidad Nacional de San Agustín (UNSA)</p>"
        "<p><b>Integrantes:</b><br>"
        "• lbacac@unsa.edu.pe<br>"
        "• gmejiar@unsa.edu.pe<br>"
        "• rponcel@unsa.edu.pe<br>"
        "• gchipanaj@unsa.edu.pe</p>"
        "<p><b>Características:</b><br>"
        "• POO Avanzada (Polimorfismo, Clases Abstractas, Funciones Virtuales)<br>"
        "• Programación Genérica y STL Memory<br>"
        "• Concurrencia y MultiThreading (std::thread, mutex, condition_variable)<br>"
        "• Arquitectura Modelo-Vista-Controlador (MVC)<br>"
        "• Patrones GoF: Factory Method y Observer<br>"
        "• Interfaz Gráfica interactiva en Qt6 con QGraphicsScene y QtCharts</p>");
}

} // namespace siger
