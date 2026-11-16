# 🚚 SIGER — Sistema Integrado de Gestión y Simulación de Entregas Rápidas

> **Trabajo Final — Tecnología de Objetos**  
> **Universidad Nacional de San Agustín de Arequipa (UNSA)**  
> **Escuela Profesional de Ingeniería de Sistemas**

---

## 👥 Equipo de Desarrollo

| Integrante | Correo Institucional | Módulo y Especialidad |
|---|---|---|
| **L. Baca C.** | `lbacac@unsa.edu.pe` | **POO Avanzada & Dominio:** Jerarquía `Vehiculo`, `Paquete`, polimorfismo y pruebas de dominio |
| **G. Mejia R.** | `gmejiar@unsa.edu.pe` | **Programación Genérica & STL:** Templates `Cola<T>`, `Repositorio<T>`, `Ruta<CoordType>`, smart pointers |
| **R. Poncel L.** | `rponcel@unsa.edu.pe` | **Concurrencia & MultiThreading:** `SimuladorEntregas`, `GeneradorEventos`, `std::mutex`, `condition_variable` |
| **G. Chipana J.** | `gchipanaj@unsa.edu.pe` | **Arquitectura MVC, Patrones GoF & GUI:** `PedidoController`, `SimulacionController`, `Factory Method`, `Observer`, Qt6 GUI |

---

## 🎯 Temas Obligatorios Implementados

1. **POO Avanzada:**
   - **Clases Abstractas:** `Vehiculo` y `Paquete` con métodos virtuales puros (`calcularCosto()`, `calcularPrioridad()`).
   - **Polimorfismo:** Implementaciones diferenciadas en `Moto`, `Auto`, `Camion`, `PaqueteEstandar`, `PaqueteFragil`.
   - **Encapsulamiento:** Variables miembro protegidas/privadas con acceso validado mediante getters/setters.

2. **Programación Genérica y STL Memory:**
   - **Templates Propios:**
     - `Cola<T>`: Cola genérica FIFO thread-safe con sincronización por `std::mutex` y métodos condicionales (`extraerSi`, `contieneSi`).
     - `Repositorio<T, Key>`: Almacén de entidades genérico con soporte para búsquedas funcionales mediante lambdas (`std::function<bool(const T&)>`).
     - `Ruta<CoordType>`: Estructura de cálculo de trayectorias euclídeas parametrizada en precisión numérica (`double`/`float`).
   - **Gestión de Memoria Moderna:** Uso estricto de `std::shared_ptr`, `std::unique_ptr` y `std::weak_ptr` (para evitar ciclos en observadores) sin fugas de memoria (RAII).

3. **Concurrencia y MultiThreading:**
   - Modelo **Productor / Consumidor** para despacho de paquetes en tiempo real.
   - Hilo independiente por cada `Repartidor` (`std::thread`).
   - Sincronización libre de race conditions con `std::mutex`, `std::unique_lock` y `std::condition_variable`.
   - **Despertar concurrente:** `notify_all()` para activación inmediata de conductores en paralelo ante nuevos pedidos.
   - **Balanceo de carga (Round-Robin voluntario):** Los conductores ceden el turno (`sleep_for`) al completar una entrega, evitando monopolio de CPU y balanceando pedidos entre toda la flota.
   - **Despacho escalonado:** Exclusión mutua `m_despachoMutex` (700 ms) para animación visual realista.
   - Contadores atómicos (`std::atomic<int>`) para métricas de rendimiento en tiempo real.

4. **Arquitectura Modelo-Vista-Controlador (MVC):**
   - **Modelo:** `GestorPedidos`, `SimuladorEntregas`, `Repositorio`, `Cola`, `ServicioRutas`.
   - **Controlador:** `PedidoController`, `SimulacionController`.
   - **Vista:** `MainWindow`, `DashboardView`, `MapaView`, `FlotaView`, `EstadisticasView`, `ControlPanel`, `FormPedido`.

5. **Patrones de Diseño GoF (5 Patrones Implementados):**
   - **Factory Method (Creacional):** Jerarquía `VehiculoFactory` (`MotoFactory`, `AutoFactory`, `CamionFactory`) y registro `FactoriaVehiculos` para instanciación desacoplada.
   - **Singleton (Creacional):** `ServicioRutas` implementado como Singleton thread-safe de Scott Meyers (C++17) para el acceso unificado al motor de ruteo vial de Arequipa (870 rutas OSRM).
   - **Strategy (Comportamiento):** `IEstrategiaTarifa` con estrategias concretas `TarifaEstandarStrategy`, `TarifaFragilStrategy` y `TarifaPesadaStrategy`, permitiendo cálculo de tarifas dinámicas en Soles (S/.).
   - **Observer / Publisher-Subscriber (Comportamiento):** `ISubject` y `IObserver` con payload `DatoEvento` para propagación de cambios del modelo hacia la interfaz gráfica en tiempo real.
   - **Model-View-Controller (Arquitectónico):** Separación estricta entre capa de datos, intermediarios de control y componentes gráficos de presentación.

6. **Interfaz Gráfica (GUI) en Qt6 — Cockpit Dashboard Unificado:**
   - **Centro de Control Unificado (Cockpit):** Mapa siempre visible en la mitad superior con divisor redimensionable `QSplitter` y atajo `F10` para alternar entre vista dividida o pestañas clásicas.
   - **MapaView 2D con Topología Vial Real:** 30 nodos metropolitanos de Arequipa, rutas reales por calles (OSRM), traza bicolor GPS (pendiente vs recorrida) y flecha chevron orientada con $\theta = \operatorname{atan2}(\Delta y, \Delta x)$.
   - **Mini-HUD Flotante:** Widget de telemetría semitransparente sobre el mapa con pedidos pendientes, móviles en ruta, nivel de tráfico y cronómetro digital.
   - **Modo Oscuro / Cockpit High-Tech (`Ctrl+D`):** Selector de tema claro/oscuro con paleta grafito nocturna, tarjetas de flota con glow cyan y renderizado de mapa oscuro.
   - **FlotaView en Vivo:** Tarjetas independientes por conductor con barra de progreso animada, estado en tiempo real y contador de entregas acumuladas.
   - **Generador de Lote Demo:** Botón `"🎲 Lote Demo (5 o 10)"` para generar instantáneamente pedidos aleatorios coherentes y realizar demostraciones en un solo clic.
   - **Persistencia JSON (`Ctrl+S` / `Ctrl+O`):** Guardado y carga completa del estado de pedidos en formato `.json`.
   - **Simulación de Tráfico Vial:** Selector de tráfico con multiplicadores cinemáticos: Fluido (1.0x), Moderado (1.25x) y Hora Punta (1.6x).
   - **Control de Velocidad:** Slider continuo de 1.0x a 4.0x sincronizado entre los hilos de simulación y la animación de mapa.

---

## 🛠️ Requisitos del Sistema

- **C++17** o superior (GCC / MinGW 13+ o MSVC 2019+)
- **CMake** (3.16 o superior)
- **Qt 6.x** (Módulos: `Core`, `Widgets`, `Charts`)
- **Ninja** o **Make**

---

## 🚀 Compilación y Ejecución

### 1. Clonar el repositorio
```bash
git clone https://github.com/ChipanaGerman/SIGER.git
cd SIGER
```

### 2. Configurar con CMake
```bash
cmake -B build -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.12.0/mingw_64"
```

### 3. Compilar la aplicación y los tests
```bash
cmake --build build
```

### 4. Ejecutar la aplicación principal
```bash
./build/SIGER.exe
```

### 5. Ejecutar la suite de pruebas unitarias
```bash
ctest --test-dir build --output-on-failure
```
O ejecutar cada prueba directamente:
```bash
./build/test_dominio.exe
./build/test_generico.exe
./build/test_threading.exe
```

---

## 📊 Cobertura de Pruebas Unitarias (CTest)

- **Dominio OOP (`test_dominio`):** 41 aserciones automatizadas (polimorfismo, herencia, encapsulamiento, capacidades, restricciones, tarifas Strategy).
- **Genérico & STL (`test_generico`):** 39 aserciones automatizadas (templates `Cola`, `Repositorio`, `Ruta`, lambdas, Singleton `ServicioRutas`, punteros inteligentes).
- **Concurrencia (`test_threading`):** 14 aserciones de estrés multihilo (productor/consumidor, balanceo de carga entre múltiples conductores, exclusión mutua, terminación limpia sin deadlocks).
- **Total:** **94 aserciones automatizadas / 3 suites de prueba con 100% de éxito.**
