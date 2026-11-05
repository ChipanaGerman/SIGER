/**
 * @file Ruta.cpp
 * @brief Instanciación explícita para el template Ruta<CoordType>.
 */
#include "Ruta.h"

namespace siger {

// Instanciaciones explícitas comunes
template class Ruta<double>;
template class Ruta<float>;

} // namespace siger
