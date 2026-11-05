#pragma once
/**
 * @file PaqueteEstandar.h
 * @brief Paquete estándar — prioridad basada en peso.
 */
#include "Paquete.h"

namespace siger {

class PaqueteEstandar : public Paquete {
public:
    PaqueteEstandar(const std::string& id,
                    const std::string& descripcion,
                    double pesoKg,
                    const std::string& direccionDestino,
                    int diasEntrega = 3);

    int         calcularPrioridad()      const override;
    std::string getTipo()                const override;
    bool        requiereManejoEspecial() const override;

    int getDiasEntrega() const { return m_diasEntrega; }

private:
    int m_diasEntrega;  // plazo estándar de entrega en días
};

} // namespace siger
