/**
 * @file test_generico.cpp
 * @brief Tests para programación genérica (templates) y gestión de memoria con smart pointers.
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>

#include "Cola.h"
#include "Repositorio.h"
#include "Ruta.h"
#include "Pedido.h"
#include "PaqueteEstandar.h"
#include "ServicioRutas.h"

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

void test_cola_generica() {
    std::cout << "\n=== Test: Cola<T> Generica Thread-Safe ===\n";

    Cola<int> colaInt;
    ASSERT_TRUE(colaInt.isEmpty(), "Cola de enteros inicia vacia");
    ASSERT_EQ(colaInt.size(), 0, "Tamano inicial es 0");

    colaInt.push(10);
    colaInt.push(20);
    colaInt.push(30);

    ASSERT_EQ(colaInt.size(), 3, "Tamano tras 3 pushes es 3");
    ASSERT_TRUE(!colaInt.isEmpty(), "Cola no esta vacia");

    auto frontVal = colaInt.peek();
    ASSERT_TRUE(frontVal.has_value(), "Peek tiene valor");
    ASSERT_EQ(frontVal.value(), 10, "Peek devuelve el primer elemento (FIFO)");

    auto pop1 = colaInt.pop();
    ASSERT_TRUE(pop1.has_value() && pop1.value() == 10, "Pop devuelve 10");
    auto pop2 = colaInt.pop();
    ASSERT_TRUE(pop2.has_value() && pop2.value() == 20, "Pop devuelve 20");
    auto pop3 = colaInt.pop();
    ASSERT_TRUE(pop3.has_value() && pop3.value() == 30, "Pop devuelve 30");

    ASSERT_TRUE(colaInt.isEmpty(), "Cola vuelve a estar vacia");
    auto popVacio = colaInt.pop();
    ASSERT_TRUE(!popVacio.has_value(), "Pop sobre cola vacia devuelve std::nullopt");

    // Probar con std::string
    Cola<std::string> colaStr;
    colaStr.push("Alfa");
    colaStr.push("Bravo");
    ASSERT_EQ(colaStr.pop().value_or(""), "Alfa", "FIFO correcto con strings");
}

void test_repositorio_generico() {
    std::cout << "\n=== Test: Repositorio<T, Key> Generico ===\n";

    Repositorio<Pedido> repo;
    ASSERT_EQ(repo.cantidad(), 0, "Repositorio inicia vacio");

    auto paq1 = std::make_shared<PaqueteEstandar>("P1", "Paquete 1", 2.5, "Calle 1");
    auto paq2 = std::make_shared<PaqueteEstandar>("P2", "Paquete 2", 5.0, "Calle 2");

    auto ped1 = std::make_shared<Pedido>("PED-01", paq1, "Origen 1", "Destino 1", 10.0);
    auto ped2 = std::make_shared<Pedido>("PED-02", paq2, "Origen 2", "Destino 2", 20.0);

    ASSERT_TRUE(repo.agregar("PED-01", ped1), "Agregar elemento 1 exitoso");
    ASSERT_TRUE(repo.agregar("PED-02", ped2), "Agregar elemento 2 exitoso");
    ASSERT_TRUE(!repo.agregar("PED-01", ped1), "No se permite agregar clave duplicada");

    ASSERT_EQ(repo.cantidad(), 2, "Cantidad es 2");
    ASSERT_TRUE(repo.existe("PED-01"), "Clave PED-01 existe");
    ASSERT_TRUE(!repo.existe("PED-99"), "Clave PED-99 no existe");

    auto obtenido = repo.obtener("PED-01");
    ASSERT_TRUE(obtenido.has_value(), "Obtener devuelve valor");
    ASSERT_EQ(obtenido.value()->getId(), "PED-01", "ID de elemento obtenido coincide");

    // Buscar con predicado lambda (programación genérica)
    auto pesados = repo.buscar([](const std::shared_ptr<Pedido>& p) {
        return p->getPaquete()->getPesoKg() > 4.0;
    });
    ASSERT_EQ(pesados.size(), 1, "Busqueda con lambda encontro 1 pedido pesado");
    ASSERT_EQ(pesados[0]->getId(), "PED-02", "El pedido pesado es PED-02");

    ASSERT_TRUE(repo.eliminar("PED-01"), "Eliminar elemento existente retorna true");
    ASSERT_EQ(repo.cantidad(), 1, "Cantidad se decrementa a 1");
    ASSERT_TRUE(!repo.eliminar("PED-01"), "Eliminar elemento ya eliminado retorna false");

    repo.limpiar();
    ASSERT_EQ(repo.cantidad(), 0, "Limpiar deja el repositorio en 0 elementos");
}

void test_ruta_template() {
    std::cout << "\n=== Test: Template Ruta<CoordType> ===\n";

    Ruta<double> ruta;
    ASSERT_EQ(ruta.size(), 0, "Ruta inicia vacia");

    ruta.agregarPunto(0.0, 0.0, "Punto A (Origen)");
    ruta.agregarPunto(3.0, 4.0, "Punto B (Intermedio)"); // distancia = 5.0
    ruta.agregarPunto(6.0, 8.0, "Punto C (Destino)");    // distancia = 5.0

    ASSERT_EQ(ruta.size(), 3, "Ruta contiene 3 puntos");
    double distTotal = ruta.calcularDistanciaTotal();
    ASSERT_TRUE(std::abs(distTotal - 10.0) < 1e-4, "Distancia euclidea total calculada es 10.0");

    // Probar con float
    Ruta<float> rutaFloat;
    rutaFloat.agregarPunto(0.0f, 0.0f, "Inicio");
    rutaFloat.agregarPunto(1.0f, 1.0f, "Fin");
    ASSERT_TRUE(rutaFloat.calcularDistanciaTotal() > 1.4f, "Distancia float calculada correctamente");
}

void test_servicio_rutas_osrm() {
    std::cout << "\n=== Test: ServicioRutas (Singleton & Topologia Vial OSRM) ===\n";

    auto& servicio = ServicioRutas::instancia();
    if (servicio.cantidadRutas() == 0) {
        servicio.inicializar("../resources/rutas_arequipa.json");
    }

    ASSERT_TRUE(servicio.cantidadRutas() > 0, "Repositorio de rutas viales cargado en memoria");
    ASSERT_TRUE(servicio.tieneRuta("Plaza de Armas", "Mall Aventura Porongoche"), "Ruta Plaza de Armas -> Mall Aventura existe");

    auto ruta = servicio.obtenerRuta("Plaza de Armas", "Mall Aventura Porongoche");
    ASSERT_TRUE(ruta.encontrada, "Ruta vial encontrada en el repositorio");
    ASSERT_TRUE(ruta.waypoints.size() > 20, "Ruta contiene mas de 20 waypoints detallados por calles");
    ASSERT_TRUE(ruta.distanciaMetros > 4000.0, "Distancia de conduccion es mayor a 4000m");

    double distKm = servicio.calcularDistanciaKm("Plaza de Armas", "Mall Aventura Porongoche");
    ASSERT_TRUE(distKm > 4.0 && distKm < 6.0, "Distancia calculada en km es coherente (~4.6 km)");

    double durMin = servicio.calcularDuracionMin("Plaza de Armas", "Mall Aventura Porongoche");
    ASSERT_TRUE(durMin > 5.0 && durMin < 15.0, "Duracion estimada es coherente (~9 min)");

    // Tolerancia a busquedas no existentes
    auto noExiste = servicio.obtenerRuta("Lugar Inexistente", "Otro Desconocido");
    ASSERT_TRUE(!noExiste.encontrada, "Ruta inexistente reportada correctamente como no encontrada");
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   SIGER — Tests Genericos & STL          ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";

    test_cola_generica();
    test_repositorio_generico();
    test_ruta_template();
    test_servicio_rutas_osrm();

    std::cout << "\n────────────────────────────────────────────\n";
    std::cout << "Resultados: " << testsPasados << " pasados, "
              << testsFallidos << " fallidos.\n";

    return testsFallidos == 0 ? 0 : 1;
}
