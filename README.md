# 🚚 SIGER — Sistema Integrado de Gestión y Simulación de Entregas Rápidas

> **Trabajo Final — Tecnología de Objetos**  
> **Universidad Nacional de San Agustín de Arequipa (UNSA)**  
> **Escuela Profesional de Ciencia de la Computación**

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
   - **Templates:**
     - `Cola<T>`: Cola genérica FIFO thread-safe con `std::mutex`.
     - `Repositorio<T, Key>`: Almacén de entidades genérico con soporte para búsquedas funcionales mediante lambdas.
     - `Ruta<CoordType>`: Estructura de cálculo de trayectorias euclídeas parametrizada en precisión numérica (`double`/`float`).
   - **Gestión de Memoria Moderna:** Uso estricto de `std::shared_ptr`, `std::unique_ptr` y `std::weak_ptr` para evitar memory leaks y ciclos de referencia.

3. **Concurrencia y MultiThreading:**
   - Modelo **Productor / Consumidor** para despacho de paquetes en tiempo real.
   - Hilo independiente por cada `Repartidor` (`std::thread`).
   - Sincronización libre de race conditions con `std::mutex`, `std::unique_lock` y `std::condition_variable`.
   - `GeneradorEventos` en segundo plano generando tráfico estocástico y pedidos concurrentes.
   - Contadores atómicos (`std::atomic<int>`) para estadísticas en tiempo real.

4. **Arquitectura Modelo-Vista-Controlador (MVC):**
   - **Modelo:** `GestorPedidos`, `SimuladorEntregas`, `Repositorio`, `Cola`.
   - **Controlador:** `PedidoController`, `SimulacionController`.
   - **Vista:** `MainWindow`, `DashboardView`, `MapaView`, `EstadisticasView`, `ControlPanel`, `FormPedido`.

5. **Patrones de Diseño GoF:**
   - **Factory Method (Creacional):** Jerarquía `VehiculoFactory` (`MotoFactory`, `AutoFactory`, `CamionFactory`) y registro `FactoriaVehiculos` para instanciación desacoplada.
   - **Observer / Publisher-Subscriber (Comportamiento):** `ISubject` y `IObserver` con payload `DatoEvento` para propagación de cambios del modelo hacia la interfaz gráfica en tiempo real.

6. **Interfaz Gráfica (GUI) en Qt6:**
   - **DashboardView:** Tabla dinámica interactiva con estados codificados por colores y cancelación de pedidos.
   - **MapaView:** Escena 2D con `QGraphicsScene` que renderiza la red vial de Arequipa y animación de repartidores en tiempo real.
   - **EstadisticasView:** Métricas clave (KPIs) y gráfico circular interactivo (`QPieSeries` de `QtCharts`).
   - **ControlPanel & FormPedido:** Controles de simulación (velocidad, pausa, reanudación) y registro interactivo de entregas.

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

## 📊 Cobertura de Pruebas Unitarias

- **Dominio OOP:** 36 pruebas automatizadas (polimorfismo, costes, capacidades, restricciones).
- **Genérico & STL:** 31 pruebas automatizadas (templates `Cola`, `Repositorio`, `Ruta`, lambdas).
- **Concurrencia:** 8 pruebas de estrés multihilo (productor/consumidor, condición de carrera, eventos).
- **Total:** 75 pruebas unitarias automatizadas con 100% de aprobación.
