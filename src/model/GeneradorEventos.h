#pragma once
/**
 * @file GeneradorEventos.h
 * @brief Generador de pedidos aleatorios en un hilo secundario para la simulación.
 *
 * Simula la llegada periódica de nuevos pedidos en el sistema SIGER,
 * registrándolos en GestorPedidos. Demuestra std::thread, std::atomic y lambdas.
 */

#include "GestorPedidos.h"
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <string>

namespace siger {

class GeneradorEventos {
public:
    /**
     * @brief Constructor del generador de eventos.
     * @param gestor Puntero débil al GestorPedidos para evitar ciclos de vida.
     * @param intervaloMs Intervalo en milisegundos entre creación de pedidos.
     */
    explicit GeneradorEventos(std::weak_ptr<GestorPedidos> gestor, int intervaloMs = 2000);

    /**
     * @brief Destructor. Detiene el hilo de forma segura.
     */
    ~GeneradorEventos();

    // Eliminar copia
    GeneradorEventos(const GeneradorEventos&) = delete;
    GeneradorEventos& operator=(const GeneradorEventos&) = delete;

    /**
     * @brief Inicia el hilo de generación periódica de pedidos.
     */
    void iniciar();

    /**
     * @brief Detiene el generador y espera a que el hilo termine.
     */
    void detener();

    /**
     * @brief Indica si el generador está activo actualmente.
     */
    bool isActivo() const noexcept { return m_corriendo.load(); }

    /**
     * @brief Modifica el intervalo de generación.
     * @param ms Milisegundos entre pedidos.
     */
    void setIntervaloMs(int ms) noexcept { m_intervaloMs = ms; }

private:
    void hiloGenerador();

    std::thread                  m_hilo;
    std::atomic<bool>            m_corriendo{false};
    std::weak_ptr<GestorPedidos> m_gestorPedidos;
    int                          m_intervaloMs{2000};
    std::atomic<int>             m_contadorId{100};
};

} // namespace siger
