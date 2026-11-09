cmake_minimum_required(VERSION 3.16)
project(SIGER VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

# ── Fuentes del Modelo ──────────────────────────────────────────────
set(MODEL_SOURCES
    src/model/Vehiculo.cpp
    src/model/Moto.cpp
    src/model/Auto.cpp
    src/model/Camion.cpp
    src/model/Paquete.cpp
    src/model/PaqueteEstandar.cpp
    src/model/PaqueteFragil.cpp
    src/model/Repartidor.cpp
    src/model/GestorPedidos.cpp
    src/model/SimuladorEntregas.cpp
    src/model/GeneradorEventos.cpp
    src/model/VehiculoFactory.cpp
    src/model/Ruta.cpp
    src/model/Pedido.cpp
    src/model/ServicioRutas.cpp
)

# ── Fuentes del Controlador ────────────────────────────────────────
set(CONTROLLER_SOURCES
    src/controller/PedidoController.cpp
    src/controller/SimulacionController.cpp
)

# ── Fuentes de la Vista (Qt) ───────────────────────────────────────
set(VIEW_SOURCES
    src/view/MainWindow.cpp
    src/view/DashboardView.cpp
    src/view/MapaView.cpp
    src/view/ControlPanel.cpp
    src/view/EstadisticasView.cpp
    src/view/FormPedido.cpp
)

# ── Ejecutable principal ───────────────────────────────────────────
add_executable(SIGER
    src/main.cpp
    ${MODEL_SOURCES}
    ${CONTROLLER_SOURCES}
    ${VIEW_SOURCES}
    resources.qrc
)

target_include_directories(SIGER PRIVATE
    src/
    src/model/
    src/controller/
    src/view/
)

target_link_libraries(SIGER PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)

# ── Tests ──────────────────────────────────────────────────────────
enable_testing()

add_executable(test_dominio    tests/test_dominio.cpp    ${MODEL_SOURCES})
add_executable(test_generico   tests/test_generico.cpp   ${MODEL_SOURCES})
add_executable(test_threading  tests/test_threading.cpp  ${MODEL_SOURCES})

foreach(test_target test_dominio test_generico test_threading)
    target_include_directories(${test_target} PRIVATE src/ src/model/)
    target_link_libraries(${test_target} PRIVATE Qt6::Core)
    add_test(NAME ${test_target} COMMAND ${test_target})
endforeach()
