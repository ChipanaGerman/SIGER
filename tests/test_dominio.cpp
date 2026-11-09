/**
 * @file test_dominio.cpp
 * @brief Tests de la jerarquía OOP: polimorfismo, herencia, encapsulamiento.
 *
 * Ejecutar con: ./test_dominio
 */
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <sstream>

#include "Vehiculo.h"
#include "Moto.h"
#include "Auto.h"
#include "Camion.h"
#include "Paquete.h"
#include "PaqueteEstandar.h"
#include "PaqueteFragil.h"
#include "Repartidor.h"
#include "Pedido.h"
#include "IEstrategiaTarifa.h"

using namespace siger;

// ── Helpers ──────────────────────────────────────────────────────────
int testsPasados = 0;
int testsFallidos = 0;

#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { \
        std::cerr << "[FAIL] " << msg << " (linea " << __LINE__ << ")\n"; \
        ++testsFallidos; \
    } else { \
        std::cout << "[PASS] " << msg << "\n"; \
        ++testsPasados; \
    }

#define ASSERT_EQ(a, b, msg) ASSERT_TRUE((a) == (b), msg)
#define ASSERT_NEAR(a, b, eps, msg) ASSERT_TRUE(std::abs((a)-(b)) < (eps), msg)

// ── Test 1: Polimorfismo en Vehiculo ─────────────────────────────────
void test_polimorfismo_vehiculo() {
    std::cout << "\n=== Test: Polimorfismo Vehiculo ===\n";

    std::vector<std::shared_ptr<Vehiculo>> flotilla;
    flotilla.push_back(std::make_shared<Moto>("V1", "ABC-001", 15.0));
    flotilla.push_back(std::make_shared<Auto>("V2", "DEF-002", 100.0));
    flotilla.push_back(std::make_shared<Camion>("V3", "GHI-003", 2000.0));

    // Polimorfismo: cada vehículo calcula su propio costo
    double costoMoto   = flotilla[0]->calcularCosto(10.0, 3.0);
    double costoAuto   = flotilla[1]->calcularCosto(10.0, 3.0);
    double costoCamion = flotilla[2]->calcularCosto(10.0, 3.0);

    ASSERT_TRUE(costoMoto < costoAuto,   "Moto más barata que Auto para misma distancia/peso");
    ASSERT_TRUE(costoAuto < costoCamion, "Auto más barato que Camión para misma distancia/peso");

    // getTipo() polimórfico
    ASSERT_EQ(flotilla[0]->getTipo(), "Moto",   "getTipo() retorna 'Moto'");
    ASSERT_EQ(flotilla[1]->getTipo(), "Auto",   "getTipo() retorna 'Auto'");
    ASSERT_EQ(flotilla[2]->getTipo(), "Camion", "getTipo() retorna 'Camion'");

    // calcularTiempoEntrega() polimórfico (moto > auto > camión en velocidad)
    double tMoto   = flotilla[0]->calcularTiempoEntrega(10.0);
    double tAuto   = flotilla[1]->calcularTiempoEntrega(10.0);
    double tCamion = flotilla[2]->calcularTiempoEntrega(10.0);
    ASSERT_TRUE(tMoto < tAuto,   "Moto más rápida que Auto en tiempo de entrega");
    ASSERT_TRUE(tAuto < tCamion, "Auto más rápido que Camión en tiempo de entrega");

    // Encapsulamiento
    ASSERT_EQ(flotilla[0]->getId(), "V1", "getId() encapsulado funciona");
    ASSERT_EQ(flotilla[0]->getEstado(), EstadoVehiculo::DISPONIBLE, "Estado inicial DISPONIBLE");
    flotilla[0]->setEstado(EstadoVehiculo::EN_RUTA);
    ASSERT_EQ(flotilla[0]->getEstado(), EstadoVehiculo::EN_RUTA, "setEstado() y getEstado() funcionan");
}

// ── Test 2: puedeCargar() polimórfico ────────────────────────────────
void test_capacidad_vehiculo() {
    std::cout << "\n=== Test: Capacidad de Carga ===\n";

    auto moto   = std::make_shared<Moto>("V1", "ABC-001", 15.0);
    auto camion = std::make_shared<Camion>("V2", "GHI-002", 2000.0);

    ASSERT_TRUE(moto->puedeCargar(10.0),    "Moto puede cargar 10 kg (< 15 kg cap)");
    ASSERT_TRUE(!moto->puedeCargar(20.0),   "Moto no puede cargar 20 kg (> 15 kg cap)");
    ASSERT_TRUE(camion->puedeCargar(1500.0),"Camión puede cargar 1500 kg");
    ASSERT_TRUE(!moto->puedeCargar(0.0),    "No se puede cargar 0 kg");
    ASSERT_TRUE(!moto->puedeCargar(-5.0),   "No se puede cargar peso negativo");
}

// ── Test 3: Polimorfismo en Paquete ──────────────────────────────────
void test_polimorfismo_paquete() {
    std::cout << "\n=== Test: Polimorfismo Paquete ===\n";

    std::vector<std::shared_ptr<Paquete>> paquetes;
    paquetes.push_back(std::make_shared<PaqueteEstandar>("P1", "Libro", 1.5, "Calle A", 3));
    paquetes.push_back(std::make_shared<PaqueteFragil>("P2", "Jarron", 0.8, "Calle B", 500.0));

    // getTipo() polimórfico
    ASSERT_EQ(paquetes[0]->getTipo(), "Estándar", "getTipo() Estándar correcto");
    ASSERT_EQ(paquetes[1]->getTipo(), "Frágil",   "getTipo() Frágil correcto");

    // requiereManejoEspecial() polimórfico
    ASSERT_TRUE(!paquetes[0]->requiereManejoEspecial(), "Estándar no requiere manejo especial");
    ASSERT_TRUE(paquetes[1]->requiereManejoEspecial(),  "Frágil SÍ requiere manejo especial");

    // calcularPrioridad() — Frágil tiene prioridad mayor
    ASSERT_TRUE(paquetes[1]->calcularPrioridad() >= paquetes[0]->calcularPrioridad(),
                "Frágil tiene prioridad >= Estándar");

    // Encapsulamiento: estado
    ASSERT_EQ(paquetes[0]->getEstado(), EstadoPaquete::PENDIENTE, "Estado inicial PENDIENTE");
    paquetes[0]->setEstado(EstadoPaquete::EN_TRANSITO);
    ASSERT_EQ(paquetes[0]->getEstado(), EstadoPaquete::EN_TRANSITO, "setEstado cambia correctamente");
    ASSERT_EQ(paquetes[0]->getEstadoStr(), "EN TRÁNSITO", "getEstadoStr() retorna texto correcto");
}

// ── Test 4: Repartidor ───────────────────────────────────────────────
void test_repartidor() {
    std::cout << "\n=== Test: Repartidor ===\n";

    auto vehiculo   = std::make_shared<Moto>("V1", "ABC-001", 15.0);
    auto repartidor = std::make_shared<Repartidor>("R001", "Carlos Quispe", vehiculo);

    ASSERT_EQ(repartidor->getEstado(), EstadoRepartidor::LIBRE, "Repartidor inicial LIBRE");
    ASSERT_TRUE(!repartidor->tienePedidosPendientes(), "Sin pedidos al inicio");

    // Crear pedido y asignar
    auto paquete = std::make_shared<PaqueteEstandar>("P1", "Zapatos", 2.0, "Av. Dolores 123", 1);
    auto pedido  = std::make_shared<Pedido>("PED-001", paquete, "Almacén Central", "Av. Dolores 123", 5.0);

    bool asignado = repartidor->asignarPedido(pedido);
    ASSERT_TRUE(asignado, "Pedido asignado exitosamente (peso dentro de capacidad)");
    ASSERT_EQ(repartidor->getEstado(), EstadoRepartidor::EN_REPARTO, "Estado EN_REPARTO tras asignación");
    ASSERT_TRUE(repartidor->tienePedidosPendientes(), "Tiene pedidos pendientes");

    // Intentar asignar paquete que excede capacidad
    auto paquetePesado = std::make_shared<PaqueteEstandar>("P2", "Nevera", 50.0, "Calle X", 5);
    auto pedidoPesado  = std::make_shared<Pedido>("PED-002", paquetePesado, "Almacén", "Calle X", 3.0);
    bool asignadoPesado = repartidor->asignarPedido(pedidoPesado);
    ASSERT_TRUE(!asignadoPesado, "Pedido con peso excesivo NO se asigna a Moto");

    // Completar pedido
    repartidor->completarPedidoActual();
    ASSERT_EQ(repartidor->getTotalEntregasCompletadas(), 1, "1 entrega completada");
    ASSERT_EQ(repartidor->getEstado(), EstadoRepartidor::LIBRE, "Repartidor LIBRE tras completar");
    ASSERT_EQ(pedido->getEstado(), EstadoPedido::COMPLETADO, "Pedido marcado como COMPLETADO");
    ASSERT_EQ(paquete->getEstado(), EstadoPaquete::ENTREGADO, "Paquete marcado como ENTREGADO");
}

// ── Test 5: Prioridad paquete urgente ────────────────────────────────
void test_prioridad_urgente() {
    std::cout << "\n=== Test: Prioridad Paquetes ===\n";

    // Paquete normal (3 días)
    auto normal  = std::make_shared<PaqueteEstandar>("P1", "Ropa", 2.0, "Dir A", 3);
    // Paquete urgente (1 día)
    auto urgente = std::make_shared<PaqueteEstandar>("P2", "Medicina", 2.0, "Dir B", 1);
    // Paquete frágil valioso
    auto fragil  = std::make_shared<PaqueteFragil>("P3", "Laptop", 1.5, "Dir C", 2000.0);

    ASSERT_TRUE(urgente->calcularPrioridad() > normal->calcularPrioridad(),
                "Urgente (1 día) tiene mayor prioridad que normal (3 días)");
    ASSERT_TRUE(fragil->calcularPrioridad() >= urgente->calcularPrioridad(),
                "Frágil valioso tiene prioridad >= urgente");
    ASSERT_TRUE(fragil->calcularPrioridad() == 10,
                "Frágil de alto valor alcanza prioridad máxima 10");
}

// ── Test 6: Patrón Strategy (GoF) en Tarifas ─────────────────────────
void test_estrategias_tarifa_strategy() {
    std::cout << "\n=== Test: Patrón Strategy (GoF) para Tarifas ===\n";

    TarifaEstandarStrategy estrategiaEstandar;
    TarifaFragilStrategy   estrategiaFragil;
    TarifaPesadaStrategy   estrategiaPesada;

    // Distancia: 10 km, Peso: 4 kg, no frágil
    double costoEst = estrategiaEstandar.calcularTarifa(10.0, 4.0, false);
    ASSERT_NEAR(costoEst, 4.00 + 10.0 * 1.20, 0.01, "Tarifa estándar calculada correctamente (S/. 16.00)");

    // Frágil con seguro
    double costoFra = estrategiaFragil.calcularTarifa(10.0, 4.0, true);
    ASSERT_TRUE(costoFra > costoEst, "Tarifa frágil con póliza es mayor a estándar");

    // Carga pesada (Camión)
    double costoPes = estrategiaPesada.calcularTarifa(10.0, 80.0, false);
    ASSERT_TRUE(costoPes > costoFra, "Tarifa pesada de 80kg es mayor a frágil");

    // Verificar integración en Pedido
    auto paq = std::make_shared<PaqueteEstandar>("P99", "Caja", 3.0, "Av. Dolores", 2);
    Pedido pedido("PED-999", paq, "Plaza de Armas", "Av. Dolores", 5.0, "Moto");

    ASSERT_TRUE(pedido.getCostoEnvio() > 0.0, "Pedido calcula costo automáticamente con Strategy");
    pedido.setCostoEnvio(25.50);
    ASSERT_EQ(pedido.getCostoEnvio(), 25.50, "SetCostoEnvio manual funciona");
}

// ── Main ─────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   SIGER — Tests de Dominio OOP           ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";

    test_polimorfismo_vehiculo();
    test_capacidad_vehiculo();
    test_polimorfismo_paquete();
    test_repartidor();
    test_prioridad_urgente();
    test_estrategias_tarifa_strategy();

    std::cout << "\n────────────────────────────────────────────\n";
    std::cout << "Resultados: " << testsPasados << " pasados, "
              << testsFallidos << " fallidos.\n";

    return testsFallidos == 0 ? 0 : 1;
}
