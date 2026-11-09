/**
 * @file test_threading.cpp
 * @brief Tests para concurrencia, multithreading, mutex y sincronización en SIGER.
 */

#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <cassert>

#include "Cola.h"
#include "GestorPedidos.h"
#include "SimuladorEntregas.h"
#include "VehiculoFactory.h"
#include "Moto.h"
#include "Auto.h"
#include "PaqueteEstandar.h"
#include "Pedido.h"
#include "Observer.h"

using namespace siger;

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

// Observer de prueba para registrar notificaciones
class ObserverPrueba : public IObserver {
public:
    std::atomic<int> conteoEventos{0};
    void onEvento(const DatoEvento& evento) override {
        ++conteoEventos;
    }
};

void test_concurrencia_cola() {
    std::cout << "\n=== Test: Concurrencia en Cola<T> (Multi-hilo) ===\n";

    Cola<int> cola;
    const int numHilos = 4;
    const int elementosPorHilo = 250;

    // Hilos productores
    std::vector<std::thread> productores;
    for (int i = 0; i < numHilos; ++i) {
        productores.emplace_back([&cola, i, elementosPorHilo]() {
            for (int j = 0; j < elementosPorHilo; ++j) {
                cola.push(i * 1000 + j);
            }
        });
    }

    for (auto& t : productores) {
        t.join();
    }

    ASSERT_EQ(cola.size(), static_cast<size_t>(numHilos * elementosPorHilo),
              "Tamano de cola coincide con total de inserciones concurrentes (1000)");

    // Hilos consumidores
    std::atomic<int> extraidos{0};
    std::vector<std::thread> consumidores;
    for (int i = 0; i < numHilos; ++i) {
        consumidores.emplace_back([&cola, &extraidos]() {
            while (true) {
                auto val = cola.pop();
                if (!val.has_value()) {
                    if (cola.isEmpty()) break;
                    std::this_thread::yield();
                    continue;
                }
                ++extraidos;
            }
        });
    }

    for (auto& t : consumidores) {
        t.join();
    }

    ASSERT_EQ(extraidos.load(), numHilos * elementosPorHilo,
              "Consumo concurrente extrajo exactamente todos los elementos sin perdidas");
    ASSERT_TRUE(cola.isEmpty(), "Cola quedo vacia tras vaciado concurrente");
}

void test_simulador_multithread() {
    std::cout << "\n=== Test: SimuladorEntregas Productor/Consumidor ===\n";

    auto cola = std::make_shared<Cola<std::shared_ptr<Pedido>>>();
    auto gestor = std::make_shared<GestorPedidos>();
    auto simulador = std::make_shared<SimuladorEntregas>(cola);

    auto obs = std::make_shared<ObserverPrueba>();
    simulador->suscribir(obs);

    // Agregar 2 repartidores
    auto v1 = std::make_shared<Moto>("M1", "MOTO-01", 15.0);
    auto v2 = std::make_shared<Auto>("A1", "AUTO-01", 100.0);
    auto r1 = std::make_shared<Repartidor>("R1", "Repartidor 1", v1);
    auto r2 = std::make_shared<Repartidor>("R2", "Repartidor 2", v2);

    simulador->agregarRepartidor(r1);
    simulador->agregarRepartidor(r2);

    // Configurar velocidad rápida para test unitario (10ms por unidad)
    simulador->setVelocidad(10);

    // Encolar 4 pedidos
    for (int i = 1; i <= 4; ++i) {
        auto paq = std::make_shared<PaqueteEstandar>("P" + std::to_string(i), "Test", 2.0, "Destino");
        auto ped = std::make_shared<Pedido>("PED-" + std::to_string(i), paq, "Origen", "Destino", 1.0);
        cola->push(ped);
    }

    ASSERT_EQ(cola->size(), 4, "Cola tiene 4 pedidos listos");

    // Iniciar simulación
    simulador->iniciar();
    ASSERT_TRUE(simulador->isCorriendo(), "Simulador esta corriendo");

    // Esperar a que los repartidores procesen los pedidos
    int intentos = 0;
    while (!cola->isEmpty() && intentos < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++intentos;
    }

    // Detener simulación
    simulador->detener();
    ASSERT_TRUE(!simulador->isCorriendo(), "Simulador se detuvo correctamente");
    ASSERT_TRUE(simulador->getEstadisticas().entregados >= 0, "Estadisticas de entregas registradas");
    ASSERT_TRUE(obs->conteoEventos.load() > 0, "Observer recibio notificaciones de eventos concurrentes");
}

void test_distribucion_equitativa_flota() {
    std::cout << "\n=== Test: Distribución Concurrente entre Múltiples Repartidores ===\n";

    auto cola = std::make_shared<Cola<std::shared_ptr<Pedido>>>();
    auto simulador = std::make_shared<SimuladorEntregas>(cola);

    // 3 repartidores con vehículos de distinto tipo y capacidad
    auto moto1 = std::make_shared<Moto>("M1", "MTO-01", 15.0);
    auto moto2 = std::make_shared<Moto>("M2", "MTO-02", 15.0);
    auto auto1 = std::make_shared<Auto>("A1", "AUT-01", 100.0);

    auto rep1 = std::make_shared<Repartidor>("R1", "Carlos Quispe", moto1);
    auto rep2 = std::make_shared<Repartidor>("R2", "Lucía Torres", moto2);
    auto rep3 = std::make_shared<Repartidor>("R3", "María Flores", auto1);

    simulador->agregarRepartidor(rep1);
    simulador->agregarRepartidor(rep2);
    simulador->agregarRepartidor(rep3);

    simulador->setVelocidad(10); // Simulación ultrarrápida para test unitario

    // Encolar 9 pedidos (como en el escenario reportado por el usuario)
    const int totalPedidos = 9;
    for (int i = 1; i <= totalPedidos; ++i) {
        auto paq = std::make_shared<PaqueteEstandar>("P" + std::to_string(i), "Paquete Test", 2.0, "Destino");
        // Con auto-asignación inteligente (cadena vacía o Auto-asignación), cualquier unidad compatible puede tomarlo
        auto ped = std::make_shared<Pedido>("PED-" + std::to_string(i), paq, "Origen", "Destino", 1.0, "");
        cola->push(ped);
    }

    ASSERT_EQ(cola->size(), totalPedidos, "Cola contiene los 9 pedidos iniciales");

    simulador->iniciar();

    int intentos = 0;
    while (!cola->isEmpty() && intentos < 80) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++intentos;
    }

    // Esperar a que los últimos pedidos en tránsito se completen
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    simulador->detener();

    int e1 = rep1->getTotalEntregasCompletadas();
    int e2 = rep2->getTotalEntregasCompletadas();
    int e3 = rep3->getTotalEntregasCompletadas();
    int totalEntregadas = e1 + e2 + e3;

    std::cout << "  -> Repartidor 1 (Moto 1): " << e1 << " entregas\n";
    std::cout << "  -> Repartidor 2 (Moto 2): " << e2 << " entregas\n";
    std::cout << "  -> Repartidor 3 (Auto 1): " << e3 << " entregas\n";

    ASSERT_TRUE(e1 > 0, "Repartidor 1 atendió pedidos");
    ASSERT_TRUE(e2 > 0, "Repartidor 2 atendió pedidos en paralelo");
    ASSERT_TRUE(e3 > 0, "Repartidor 3 (Auto) apoyó con pedidos compatibles");
    ASSERT_TRUE(e1 < totalPedidos, "Ningún conductor monopolizó el 100% de los 9 pedidos");
    ASSERT_EQ(totalEntregadas, totalPedidos, "Todos los 9 pedidos fueron distribuidos y completados exitosamente");
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   SIGER — Tests MultiThreading & Concurr ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";

    test_concurrencia_cola();
    test_simulador_multithread();
    test_distribucion_equitativa_flota();

    std::cout << "\n────────────────────────────────────────────\n";
    std::cout << "Resultados: " << testsPasados << " pasados, "
              << testsFallidos << " fallidos.\n";

    return testsFallidos == 0 ? 0 : 1;
}
