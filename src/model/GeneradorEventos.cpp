/**
 * @file GeneradorEventos.cpp
 * @brief Implementación del generador de eventos para SIGER.
 */

#include "GeneradorEventos.h"
#include "PaqueteEstandar.h"
#include "PaqueteFragil.h"
#include "Pedido.h"

#include <chrono>
#include <random>
#include <sstream>
#include <vector>

namespace siger {

GeneradorEventos::GeneradorEventos(std::weak_ptr<GestorPedidos> gestor, int intervaloMs)
    : m_gestorPedidos(std::move(gestor))
    , m_intervaloMs(intervaloMs)
{}

GeneradorEventos::~GeneradorEventos() {
    detener();
}

void GeneradorEventos::iniciar() {
    if (m_corriendo.exchange(true)) {
        return; // Ya estaba corriendo
    }
    m_hilo = std::thread(&GeneradorEventos::hiloGenerador, this);
}

void GeneradorEventos::detener() {
    if (!m_corriendo.exchange(false)) {
        return; // Ya estaba detenido
    }
    if (m_hilo.joinable()) {
        m_hilo.join();
    }
}

void GeneradorEventos::hiloGenerador() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distPeso(0.5, 45.0);
    std::uniform_real_distribution<double> distKm(1.0, 25.0);
    std::uniform_real_distribution<double> distValor(100.0, 2500.0);
    std::uniform_int_distribution<int>     distTipo(0, 1);
    std::uniform_int_distribution<int>     distDias(1, 5);

    // 20 ubicaciones emblemáticas de Arequipa
    const std::vector<std::string> ubicaciones = {
        "Plaza de Armas",
        "Calle Mercaderes 120",
        "Calle San Francisco 204",
        "Mirador de Yanahuara",
        "Av. Ejercito 450",
        "Av. Cayma 310",
        "Urb. Umacollo B-12",
        "UNSA Ingenierias (Av. Independencia)",
        "Av. Venezuela 800 (UNSA)",
        "Av. Lambramani 105",
        "Mall Aventura Porongoche",
        "Av. Dolores 890",
        "Plaza Mayta Capac (Miraflores)",
        "Parque Selva Alegre",
        "Mirador de Sachaca",
        "Terminal Terrestre Arequipa",
        "Cerro Colorado (Plaza Las Americas)",
        "Parque Industrial",
        "Plaza de Paucarpata",
        "Molino de Sabandia"
    };
    std::uniform_int_distribution<size_t> distIdx(0, ubicaciones.size() - 1);

    auto generarId = [this]() {
        std::ostringstream oss;
        oss << "PED-" << ++m_contadorId;
        return oss.str();
    };

    while (m_corriendo.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_intervaloMs));
        if (!m_corriendo.load()) break;

        auto gestor = m_gestorPedidos.lock();
        if (!gestor) {
            break;
        }

        std::string id = generarId();
        double peso = distPeso(gen);
        double distancia = distKm(gen);

        size_t idxOrigen = distIdx(gen);
        size_t idxDestino = distIdx(gen);
        if (idxDestino == idxOrigen) {
            idxDestino = (idxOrigen + 1) % ubicaciones.size();
        }

        std::string origen = ubicaciones[idxOrigen];
        std::string destino = ubicaciones[idxDestino];

        std::shared_ptr<Paquete> paquete;
        if (distTipo(gen) == 0) {
            paquete = std::make_shared<PaqueteEstandar>(
                "PAQ-" + id,
                "Mercancía estándar (" + std::to_string(static_cast<int>(peso)) + " kg)",
                peso,
                destino,
                distDias(gen)
            );
        } else {
            paquete = std::make_shared<PaqueteFragil>(
                "PAQ-" + id,
                "Carga frágil y delicada",
                peso,
                destino,
                distValor(gen)
            );
        }

        auto nuevoPedido = std::make_shared<Pedido>(id, paquete, origen, destino, distancia);
        gestor->registrarPedido(nuevoPedido);
    }
}

} // namespace siger
