/**
 * @file MainWindow.cpp
 * @brief Implementación de la ventana principal de SIGER.
 */

#include "MainWindow.h"
#include "FlotaView.h"
#include "ServicioRutas.h"
#include "IEstrategiaTarifa.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>

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
    resize(1340, 860);

    // ── Centro de Control Unificado (Cockpit Dashboard) ───────────────
    // Usamos un QSplitter vertical para que el Mapa de Arequipa esté siempre
    // visible en la parte superior y el panel de supervisión (tabla, flota, métricas)
    // esté visible en la parte inferior de manera simultánea sin fatiga de pestañas.
    m_splitterPrincipal = new QSplitter(Qt::Vertical, this);
    m_splitterPrincipal->setChildrenCollapsible(false);

    m_mapaView = new MapaView(this);
    m_dashboardView = new DashboardView(this);
    m_flotaView = new FlotaView(this);
    m_estadisticasView = new EstadisticasView(this);

    // 1. Zona Superior: Mapa Interactivo en Vivo
    m_splitterPrincipal->addWidget(m_mapaView);

    // 2. Zona Inferior: Panel de Supervisión Multi-vista
    m_tabPanelInferior = new QTabWidget(this);
    m_tabPanelInferior->addTab(m_dashboardView, "📋 Pedidos Activos (Dashboard)");
    m_tabPanelInferior->addTab(m_flotaView, "🚚 Monitoreo de Flota");
    m_tabPanelInferior->addTab(m_estadisticasView, "📊 Estadísticas y Gráficos");
    m_splitterPrincipal->addWidget(m_tabPanelInferior);

    // Proporciones iniciales: 520px para el Mapa (62%), 320px para la Supervisión (38%)
    m_splitterPrincipal->setSizes({520, 320});

    setCentralWidget(m_splitterPrincipal);

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
    auto actGuardar = menuArchivo->addAction("💾 Guardar Sesión (JSON)...");
    actGuardar->setShortcut(QKeySequence::Save);
    connect(actGuardar, &QAction::triggered, this, &MainWindow::slotGuardarSesionJson);

    auto actCargar = menuArchivo->addAction("📂 Cargar Sesión (JSON)...");
    actCargar->setShortcut(QKeySequence::Open);
    connect(actCargar, &QAction::triggered, this, &MainWindow::slotCargarSesionJson);

    menuArchivo->addSeparator();
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

    auto menuVer = menuBar()->addMenu("&Ver");
    auto actModoOscuro = menuVer->addAction("🌙 Alternar Modo Oscuro (Dark Theme)");
    actModoOscuro->setShortcut(QKeySequence("Ctrl+D"));
    connect(actModoOscuro, &QAction::triggered, this, &MainWindow::slotToggleModoOscuro);

    auto actTogglePanel = menuVer->addAction("🔲 Alternar Panel Inferior (Maximizar Mapa)");
    actTogglePanel->setShortcut(QKeySequence("F10"));
    connect(actTogglePanel, &QAction::triggered, this, &MainWindow::slotTogglePanelInferior);

    auto menuAyuda = menuBar()->addMenu("A&yuda");
    auto actAcercaDe = menuAyuda->addAction("Acerca de SIGER");
    connect(actAcercaDe, &QAction::triggered, this, &MainWindow::onAcercaDe);

    // Timer de cronómetro para el Mini-HUD flotante
    m_timerCronometro = new QTimer(this);
    connect(m_timerCronometro, &QTimer::timeout, this, &MainWindow::onTimerCronometroTimeout);
    m_timerCronometro->start(1000);

    // Barra de estado
    statusBar()->showMessage("SIGER Listo. Centro de Control Unificado — UNSA Tecnología de Objetos");
}

void MainWindow::slotTogglePanelInferior() {
    if (m_tabPanelInferior) {
        bool visible = m_tabPanelInferior->isVisible();
        m_tabPanelInferior->setVisible(!visible);
    }
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

    // Actualización del panel de flota en tiempo real
    connect(m_pedidoCtrl.get(), &PedidoController::pedidoEnCamino,
            m_flotaView, &FlotaView::slotPedidoEnCamino);

    connect(m_simulacionCtrl.get(), &SimulacionController::entregaCompletada,
            m_flotaView, &FlotaView::slotPedidoCompletado);

    connect(m_controlPanel, &ControlPanel::sigDetener,
            m_flotaView, &FlotaView::slotResetearFlota);

    connect(m_controlPanel, &ControlPanel::sigGenerarLoteDemo,
            this, &MainWindow::slotGenerarLoteDemo);

    connect(m_controlPanel, &ControlPanel::sigTraficoCambiado,
            this, [this](double factor) {
                if (factor > 1.4) {
                    m_descTrafico = "🔴 Hora Punta (1.6x)";
                } else if (factor > 1.1) {
                    m_descTrafico = "🟡 Moderado (1.25x)";
                } else {
                    m_descTrafico = "🟢 Fluido (1.0x)";
                }
                m_simulacionCtrl->slotSetTrafico(factor);
                m_mapaView->setFactorTrafico(factor);
                m_mapaView->actualizarHud(m_pedidosEnCola, m_pedidosEnRuta, m_descTrafico, "");
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
            this, [this](int ms) {
                m_simulacionCtrl->slotSetVelocidad(ms);
                double factor = static_cast<double>(ms) / 1000.0;
                m_mapaView->setFactorVelocidad(factor);
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::simulacionIniciada,
            this, [this]() {
                m_segundosSimulacion = 0;
                statusBar()->showMessage("Simulación en ejecución activa...");
            });

    connect(m_pedidoCtrl.get(), &PedidoController::estadisticasActualizadas,
            this, [this](int total, int pendientes, int completados, int cancelados, double facturado) {
                Q_UNUSED(total);
                Q_UNUSED(completados);
                Q_UNUSED(cancelados);
                Q_UNUSED(facturado);
                m_pedidosEnCola = pendientes;
                m_mapaView->actualizarHud(m_pedidosEnCola, m_pedidosEnRuta, m_descTrafico, "");
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::estadoActualizado,
            this, [this](int entregados, int fallidos, int enCamino) {
                Q_UNUSED(entregados);
                Q_UNUSED(fallidos);
                m_pedidosEnRuta = enCamino;
                m_mapaView->actualizarHud(m_pedidosEnCola, m_pedidosEnRuta, m_descTrafico, "");
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::simulacionDetenida,
            m_controlPanel, &ControlPanel::slotSimulacionTerminada);

    connect(m_simulacionCtrl.get(), &SimulacionController::simulacionDetenida,
            this, [this]() {
                statusBar()->showMessage("Simulación detenida / lista para nuevos pedidos.");
                m_mapaView->resetearMapa();
            });

    connect(m_simulacionCtrl.get(), &SimulacionController::todosCompletados,
            m_controlPanel, &ControlPanel::slotSimulacionTerminada);

    connect(m_simulacionCtrl.get(), &SimulacionController::todosCompletados,
            this, [this]() {
                statusBar()->showMessage("✅ ¡Todos los pedidos completados! Puedes agregar más pedidos e iniciar otra simulación.");
                QMessageBox::information(this, "Simulación completada",
                    "<b>¡Todas las entregas han sido completadas con éxito!</b><br>"
                    "La simulación ha finalizado.<br><br>"
                    "El sistema está listo: puedes registrar nuevos pedidos en el panel derecho "
                    "y pulsar <b>▶ Iniciar Simulación</b> nuevamente sin necesidad de reiniciar el programa.");
            });
}

void MainWindow::onAcercaDe() {
    QMessageBox::about(this, "Acerca de SIGER",
        "<h3>SIGER v2.0</h3>"
        "<p><b>Sistema Integrado de Gestión y Simulación de Entregas Rápidas</b></p>"
        "<p>Curso: <i>Tecnología de Objetos</i><br>"
        "Universidad Nacional de San Agustín (UNSA)</p>"
        "<p><b>Integrantes:</b><br>"
        "• lbacac@unsa.edu.pe<br>"
        "• gmejiar@unsa.edu.pe<br>"
        "• rponcel@unsa.edu.pe<br>"
        "• gchipanaj@unsa.edu.pe</p>"
        "<p><b>Características Implementadas:</b><br>"
        "• POO Avanzada (Polimorfismo, Clases Abstractas, Métodos Virtuales)<br>"
        "• Patrones GoF: Factory Method, Observer, Singleton, Strategy<br>"
        "• Programación Genérica (Cola&lt;T&gt;, Repositorio&lt;T&gt;, Ruta&lt;Coord&gt;) y Smart Pointers RAII<br>"
        "• Concurrencia Multi-hilo (std::thread por conductor, exclusión mutua, notify_all)<br>"
        "• Arquitectura Modelo-Vista-Controlador (MVC)<br>"
        "• Topología Vial Real de Arequipa (OpenStreetMap / OSRM con 30 nodos y 870 rutas)<br>"
        "• Cockpit Dashboard Unificado con Telemetría Mini-HUD y Modo Oscuro</p>");
}

void MainWindow::slotGenerarLoteDemo(int cantidad) {
    if (!m_pedidoCtrl) return;

    const QStringList ubicaciones = {
        "Plaza de Armas", "Calle Mercaderes 120", "Calle San Francisco 204",
        "Mirador de Yanahuara", "Av. Ejercito 450", "Av. Cayma 310",
        "Urb. Umacollo B-12", "UNSA Ingenierias (Av. Independencia)", "Av. Venezuela 800 (UNSA)",
        "Av. Lambramani 105", "Mall Aventura Porongoche", "Av. Dolores 890",
        "Plaza Mayta Capac (Miraflores)", "Parque Selva Alegre", "Mirador de Sachaca",
        "Terminal Terrestre Arequipa", "Cerro Colorado (Plaza Las Americas)", "Parque Industrial",
        "Plaza de Paucarpata", "Molino de Sabandia", "Monasterio de Santa Catalina",
        "Mercado San Camilo", "Hospital Regional Honorio Delgado", "Hospital Goyeneche",
        "Estadio Melgar (IV Centenario)", "Real Plaza Arequipa (Cayma)", "Plaza de Cayma (San Miguel)",
        "Plataforma Andres Avelino Caceres", "Balneario de Tingo", "Urb. Vallecito"
    };

    struct PlantillaPedido {
        QString descripcion;
        double peso;
        QString vehiculo;
        bool esFragil;
    };

    const std::vector<PlantillaPedido> plantillas = {
        { "Insumos médicos y vacunas urgentes", 2.5, "Moto", true },
        { "Documentos notariales y contratos", 1.2, "Moto", false },
        { "Medicinas para Hospital Honorio Delgado", 3.0, "Moto", true },
        { "Componentes electrónicos y chips", 4.0, "Moto", true },
        { "Lote de laptops corporativas UNSA", 28.0, "Auto", true },
        { "Repuestos automotrices y filtros", 45.0, "Auto", false },
        { "Cajas de libros y material didáctico", 35.0, "Auto", false },
        { "Equipos de telecomunicaciones", 55.0, "Auto", true },
        { "Mobiliario ergonómico y sillas de oficina", 180.0, "Camión", false },
        { "Electrodomésticos y refrigeración", 320.0, "Camión", true },
        { "Cargamento de reactivos químicos", 250.0, "Camión", true },
        { "Materiales de construcción y herramientas", 450.0, "Camión", false }
    };

    auto* rng = QRandomGenerator::global();
    int totalRegistrados = 0;

    for (int i = 0; i < cantidad; ++i) {
        int idxPlantilla = (i < static_cast<int>(plantillas.size()))
            ? i
            : rng->bounded(static_cast<int>(plantillas.size()));

        const auto& item = plantillas[idxPlantilla];

        int idxOrigen = rng->bounded(ubicaciones.size());
        int idxDestino = (idxOrigen + 1 + rng->bounded(ubicaciones.size() - 1)) % ubicaciones.size();

        QString origen = ubicaciones[idxOrigen];
        QString destino = ubicaciones[idxDestino];

        double distKm = ServicioRutas::instancia().calcularDistanciaKm(origen, destino);
        if (distKm <= 0.2) distKm = 3.5;

        double costo = 0.0;
        if (item.peso > 100.0) {
            costo = TarifaPesadaStrategy().calcularTarifa(distKm, item.peso, item.esFragil);
        } else if (item.esFragil) {
            costo = TarifaFragilStrategy().calcularTarifa(distKm, item.peso, item.esFragil);
        } else {
            costo = TarifaEstandarStrategy().calcularTarifa(distKm, item.peso, item.esFragil);
        }

        QString tipoStr = item.esFragil ? "Frágil (Cuidado especial)" : "Estándar";
        QString vehiculoStr = item.vehiculo + " (" + QString::number(item.peso, 'f', 1) + "kg)";

        m_pedidoCtrl->slotAgregarPedido(tipoStr, item.descripcion, item.peso, origen, destino, distKm, vehiculoStr, costo);
        totalRegistrados++;
    }

    statusBar()->showMessage(QString("🎲 Lote demo de %1 pedidos generado exitosamente. ¡Listos para iniciar!").arg(totalRegistrados), 5000);
    QMessageBox::information(this, "Lote Demo Generado",
        QString("<b>Se han generado %1 pedidos de demostración en Arequipa</b><br><br>"
                "• Los pedidos abarcan Motos, Autos y Camiones de forma equilibrada.<br>"
                "• Rutas viales reales OSRM y tarifas calculadas con el patrón Strategy.<br>"
                "• Pulsa <b>▶ Iniciar Simulación</b> para ver el despacho concurrente.").arg(totalRegistrados));
}

void MainWindow::slotGuardarSesionJson() {
    if (!m_pedidoCtrl) return;

    QString rutaArchivo = QFileDialog::getSaveFileName(
        this,
        "Guardar Sesión de Entregas SIGER",
        "sesion_siger.json",
        "Archivos JSON (*.json);;Todos los archivos (*.*)"
    );

    if (rutaArchivo.isEmpty()) return;

    QJsonObject sesionJson;
    sesionJson["aplicacion"] = "SIGER";
    sesionJson["version"] = "2.0.0";
    sesionJson["curso"] = "Tecnologia de Objetos - UNSA";
    sesionJson["fecha"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray pedidosArray;
    auto listaPedidos = m_pedidoCtrl->getTodosLosPedidos();
    for (const auto& ped : listaPedidos) {
        if (!ped) continue;
        QJsonObject pedObj;
        pedObj["id"] = QString::fromStdString(ped->getId());
        pedObj["origen"] = QString::fromStdString(ped->getOrigen());
        pedObj["destino"] = QString::fromStdString(ped->getDestino());
        pedObj["distanciaKm"] = ped->getDistanciaKm();
        pedObj["costoEnvio"] = ped->getCostoEnvio();
        pedObj["estado"] = QString::fromStdString(ped->getEstadoStr());
        pedObj["vehiculoRequerido"] = QString::fromStdString(ped->getTipoVehiculoRequerido());
        pedObj["repartidorId"] = QString::fromStdString(ped->getRepartidorId());

        if (ped->getPaquete()) {
            pedObj["tipoPaquete"] = QString::fromStdString(ped->getPaquete()->getTipo());
            pedObj["descripcion"] = QString::fromStdString(ped->getPaquete()->getDescripcion());
            pedObj["pesoKg"] = ped->getPaquete()->getPesoKg();
        }
        pedidosArray.append(pedObj);
    }

    sesionJson["totalPedidos"] = static_cast<int>(listaPedidos.size());
    sesionJson["pedidos"] = pedidosArray;

    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error al guardar", "No se pudo escribir en el archivo seleccionado.");
        return;
    }

    QJsonDocument doc(sesionJson);
    archivo.write(doc.toJson(QJsonDocument::Indented));
    archivo.close();

    statusBar()->showMessage("💾 Sesión guardada exitosamente en: " + rutaArchivo, 5000);
    QMessageBox::information(this, "Sesión Guardada",
        QString("<b>Sesión exportada con éxito</b><br><br>"
                "Se guardaron %1 pedidos con sus rutas, costos y estados en:<br><code>%2</code>")
            .arg(listaPedidos.size()).arg(rutaArchivo));
}

void MainWindow::slotCargarSesionJson() {
    if (!m_pedidoCtrl) return;

    QString rutaArchivo = QFileDialog::getOpenFileName(
        this,
        "Cargar Sesión de Entregas SIGER",
        "",
        "Archivos JSON (*.json);;Todos los archivos (*.*)"
    );

    if (rutaArchivo.isEmpty()) return;

    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error al abrir", "No se pudo leer el archivo seleccionado.");
        return;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(datos, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::critical(this, "Error de formato", "El archivo no contiene un JSON válido de SIGER: " + parseError.errorString());
        return;
    }

    QJsonObject sesionJson = doc.object();
    QJsonArray pedidosArray = sesionJson["pedidos"].toArray();

    int importados = 0;
    for (const auto& val : pedidosArray) {
        QJsonObject pObj = val.toObject();
        QString tipo = pObj.value("tipoPaquete").toString("Estándar");
        QString desc = pObj.value("descripcion").toString("Envío importado");
        double peso = pObj.value("pesoKg").toDouble(2.0);
        QString origen = pObj.value("origen").toString("Plaza de Armas");
        QString destino = pObj.value("destino").toString("Mall Aventura Porongoche");
        double distancia = pObj.value("distanciaKm").toDouble(4.5);
        QString vehiculo = pObj.value("vehiculoRequerido").toString("Moto");
        double costo = pObj.value("costoEnvio").toDouble(0.0);

        m_pedidoCtrl->slotAgregarPedido(tipo, desc, peso, origen, destino, distancia, vehiculo, costo);
        importados++;
    }

    statusBar()->showMessage(QString("📂 Sesión cargada: %1 pedidos importados.").arg(importados), 5000);
    QMessageBox::information(this, "Sesión Cargada",
        QString("<b>Sesión importada exitosamente</b><br><br>"
                "Se cargaron %1 pedidos listos para inspección o simulación.")
            .arg(importados));
}

void MainWindow::slotToggleModoOscuro() {
    m_modoOscuro = !m_modoOscuro;
    aplicarTema(m_modoOscuro);
}

void MainWindow::aplicarTema(bool oscuro) {
    if (oscuro) {
        QString darkQss = R"(
            QMainWindow, QWidget#centralWidget {
                background-color: #1A1D24;
                color: #ECEFF1;
            }
            QMenuBar {
                background-color: #14171E;
                color: #ECEFF1;
                border-bottom: 1px solid #2C3240;
            }
            QMenuBar::item:selected {
                background-color: #2C3240;
                border-radius: 4px;
            }
            QMenu {
                background-color: #1A1D24;
                color: #ECEFF1;
                border: 1px solid #2C3240;
            }
            QMenu::item:selected {
                background-color: #00BCD4;
                color: black;
            }
            QDockWidget {
                color: #00BCD4;
                font-weight: bold;
            }
            QDockWidget::title {
                background: #14171E;
                padding: 6px;
                border-bottom: 1px solid #2C3240;
            }
            QTabWidget::pane {
                border: 1px solid #2C3240;
                background: #1A1D24;
            }
            QTabBar::tab {
                background: #14171E;
                color: #B0BEC5;
                padding: 8px 16px;
                margin-right: 2px;
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
            }
            QTabBar::tab:selected {
                background: #252A34;
                color: #00BCD4;
                font-weight: bold;
                border-bottom: 2px solid #00BCD4;
            }
            QTableWidget {
                background-color: #1E222B;
                alternate-background-color: #252A34;
                color: #ECEFF1;
                gridline-color: #2C3240;
                border: 1px solid #2C3240;
            }
            QHeaderView::section {
                background-color: #14171E;
                color: #00BCD4;
                font-weight: bold;
                padding: 5px;
                border: 1px solid #2C3240;
            }
            QLineEdit, QComboBox, QDoubleSpinBox, QSpinBox {
                background-color: #252A34;
                color: #ECEFF1;
                border: 1px solid #37474F;
                border-radius: 4px;
                padding: 4px;
            }
            QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus {
                border: 1px solid #00BCD4;
            }
            QStatusBar {
                background-color: #14171E;
                color: #90A4AE;
                border-top: 1px solid #2C3240;
            }
        )";

        qApp->setStyleSheet(darkQss);
        if (m_mapaView) m_mapaView->setModoOscuro(true);
        if (m_flotaView) m_flotaView->setModoOscuro(true);
        statusBar()->showMessage("🌙 Modo Oscuro Activado (High-Tech Cyber Cockpit)", 3000);
    } else {
        qApp->setStyleSheet("");
        if (m_mapaView) m_mapaView->setModoOscuro(false);
        if (m_flotaView) m_flotaView->setModoOscuro(false);
        statusBar()->showMessage("☀️ Modo Claro Activado", 3000);
    }
}

void MainWindow::onTimerCronometroTimeout() {
    if (m_simulacionCtrl && m_simulacionCtrl->getSimulador() && m_simulacionCtrl->getSimulador()->isCorriendo()) {
        m_segundosSimulacion++;
        int mins = m_segundosSimulacion / 60;
        int segs = m_segundosSimulacion % 60;
        QString tiempoStr = QString("%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(segs, 2, 10, QChar('0'));

        if (m_mapaView) {
            m_mapaView->actualizarHud(m_pedidosEnCola, m_pedidosEnRuta, m_descTrafico, tiempoStr);
        }
    }
}

} // namespace siger
