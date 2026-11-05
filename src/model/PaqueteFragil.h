#pragma once
/**
 * @file PaqueteFragil.h
 * @brief Paquete frágil — alta prioridad, requiere manejo especial.
 *
 * Demuestra polimorfismo: calcularPrioridad() y requiereManejoEspecial()
 * tienen comportamiento distinto al de PaqueteEstandar.
 */
#include "Paquete.h"

namespace siger {

class PaqueteFragil : public Paquete {
public:
    PaqueteFragil(const std::string& id,
                  const std::string& descripcion,
                  double pesoKg,
                  const std::string& direccionDestino,
                  double valorAsegurado);

    int         calcularPrioridad()      const override;
    std::string getTipo()                const override;
    bool        requiereManejoEspecial() const override;

    double getValorAsegurado() const { return m_valorAsegurado; }

private:
    double m_valorAsegurado;  // valor declarado en soles (afecta prioridad)
};

} // namespace siger
