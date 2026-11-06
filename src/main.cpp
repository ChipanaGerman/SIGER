/**
 * @file main.cpp
 * @brief Punto de entrada principal para el sistema SIGER.
 */

#include <QApplication>
#include <memory>

#include "VehiculoFactory.h"
#include "Repartidor.h"
#include "Cola.h"
#include "GestorPedidos.h"
#include "SimuladorEntregas.h"
#include "GeneradorEventos.h"
#include "PedidoController.h"
#include "SimulacionController.h"
#include "MainWindow.h"

using namespace siger;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SIGER");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("UNSA - Tecnología de Objetos");

    // 1. Configurar Patrón GoF Factory Method
    FactoriaVehiculos::registrar("Moto",   std::make_unique<MotoFactory>());
    FactoriaVehiculos::registrar("Auto",   std::make_unique<AutoFactory>());
    FactoriaVehiculos::registrar("Camion", std::make_unique<CamionFactory>());

    // Crear flota inicial usando Factory Method (Motos, Autos y Camiones)
    auto moto1   = FactoriaVehiculos::crear("Moto",   "V-01", "MTO-501", 15.0);
    auto moto2   = FactoriaVehiculos::crear("Moto",   "V-02", "MTO-502", 15.0);
    auto auto1   = FactoriaVehiculos::crear("Auto",   "V-03", "AUT-701", 100.0);
    auto auto2   = FactoriaVehiculos::crear("Auto",   "V-04", "AUT-702", 100.0);
    auto camion1 = FactoriaVehiculos::crear("Camion", "V-05", "CAM-901", 2000.0);

    // 2. Crear Repartidores con sus respectivos vehículos
    auto rep1 = std::make_shared<Repartidor>("R-101", "Carlos Quispe (Moto 1)", moto1);
    auto rep2 = std::make_shared<Repartidor>("R-102", "Lucía Torres (Moto 2)",  moto2);
    auto rep3 = std::make_shared<Repartidor>("R-103", "María Flores (Auto 1)",  auto1);
    auto rep4 = std::make_shared<Repartidor>("R-104", "Jorge Ramos (Auto 2)",   auto2);
    auto rep5 = std::make_shared<Repartidor>("R-105", "Pedro Mamani (Camión)",  camion1);

    // 3. Crear Estructuras del Modelo
    auto gestorPedidos = std::make_shared<GestorPedidos>();
    auto colaCompartida = gestorPedidos->getColaEspera();

    // 4. Crear Simulador Multihilo y Generador de Eventos
    auto simulador = std::make_shared<SimuladorEntregas>(colaCompartida);
    simulador->agregarRepartidor(rep1);
    simulador->agregarRepartidor(rep2);
    simulador->agregarRepartidor(rep3);
    simulador->agregarRepartidor(rep4);
    simulador->agregarRepartidor(rep5);

    auto generador = std::make_shared<GeneradorEventos>(gestorPedidos, 2500);

    // 5. Crear Controladores MVC
    auto pedidoCtrl = std::make_shared<PedidoController>(gestorPedidos);
    auto simulacionCtrl = std::make_shared<SimulacionController>(simulador, generador);

    // 6. Suscribir Observadores (Patrón GoF Observer)
    gestorPedidos->suscribir(pedidoCtrl);
    simulador->suscribir(simulacionCtrl);
    simulador->suscribir(pedidoCtrl);
    simulador->suscribir(gestorPedidos);   // GestorPedidos contabiliza los pedidos completados
    gestorPedidos->suscribir(simulador);   // Simulador despierta repartidores ante nuevos pedidos
    gestorPedidos->suscribir(simulacionCtrl); // SimulacionCtrl detecta TODOS_COMPLETADOS

    // 7. Iniciar Interfaz Gráfica
    MainWindow window(pedidoCtrl, simulacionCtrl);
    window.show();

    return app.exec();
}
