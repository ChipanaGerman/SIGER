# 📘 GUÍA MAESTRA DE SUSTENTACIÓN — SIGER
## Todo lo que necesitas saber, entender y dominar para sacar 20
**Estudiante:** German Arturo Chipana Jeronimo  
**Proyecto:** SIGER (Sistema Integrado de Gestión y Simulación de Entregas Rápidas)  
**Curso:** Tecnología de Objetos — UNSA, 2026  

---

# 🧠 1. ¿Qué es SIGER en palabras sencillas?

Imagina una mezcla entre **PedidosYa / Uber Eats** y un **simulador de tráfico en tiempo real**, pero construido desde cero en **C++17 y Qt6**, funcionando sobre el **mapa real de la ciudad de Arequipa**.

### ¿Qué hace exactamente?
1. Tú registras pedidos de entrega (o presionas un botón para que se creen 10 pedidos automáticos). Cada pedido tiene un origen en Arequipa (ej. *Plaza de Armas*), un destino (ej. *Mall Aventura Porongoche*), un peso en kilos y un tipo (estándar o frágil).
2. El sistema calcula automáticamente la **tarifa en Soles** y la **distancia real por calles** (no una línea recta imaginaria, sino doblando esquinas reales con OpenStreetMap).
3. Cuando le das a **Iniciar Simulación**, se despiertan **5 conductores** en segundo plano (2 Motos, 2 Autos y 1 Camión). Cada conductor es un **hilo independiente de tu computadora**.
4. Los conductores van tomando pedidos compatibles con su vehículo, salen a la calle en el mapa visual, recorren la ruta esquivando o sufriendo el tráfico que tú elijas, y al llegar lanzan un pulso de luz en el destino mientras se acumulan estadísticas de dinero ganado y tiempos de entrega.

---

# 🔄 2. El viaje de un pedido: ¿Cómo funciona todo por dentro?

Para que lo entiendas como una película, este es el viaje de un pedido desde que nace hasta que se entrega:

```
[Usuario / Lote Demo] 
       │ 
       ▼ (1. Registra pedido)
[PedidoController] ──> Valida datos y llama a la fábrica de tarifas
       │
       ▼ (2. Guarda y encola)
[GestorPedidos] ──> Guarda en Repositorio<Pedido> y mete a la Cola<T>
       │
       ▼ (3. Avisa con campanita: notify_all)
[SimuladorEntregas] (Monitor Multihilo)
       │
       ├─► [Hilo Moto 1]   (Revisa si puede cargar el peso)
       ├─► [Hilo Moto 2]   (Compiten limpiamente con mutex)
       ├─► [Hilo Auto 1]   (Toma su pedido y desbloquea la cola)
       │
       ▼ (4. Conduce por las calles de Arequipa)
[ServicioRutas (Singleton OSRM)] ──> Da los waypoints (curva por curva)
       │
       ▼ (5. Notifica eventos al Observer)
[MapaView & FlotaView] ──> Pintan el coche moviéndose y la barra de progreso
       │
       ▼ (6. Llega a destino)
[Efecto Ripple & Estadísticas] ──> Onda expansiva, suma Soles y cede el turno
```

### Paso a paso detallado:
1. **Nacimiento:** Se crea un `Pedido` con ID (ej. `PED-501`), origen, destino y peso.
2. **Cálculo de precio:** Antes de guardarlo, el patrón **Strategy** evalúa el peso y si es frágil o estándar, calculando el precio exacto en Soles.
3. **A la cola de espera:** El pedido entra a nuestra estructura genérica propia: `Cola<std::shared_ptr<Pedido>>`.
4. **Despertar a la flota:** El simulador ejecuta `m_condVar.notify_all()`. Esto despierta a los 5 hilos de los choferes que estaban "durmiendo" sin gastar batería ni procesador.
5. **Selección inteligente:** Cada chofer le pregunta a la cola con una función lambda: *"¿Tienes algún pedido que no supere mi peso máximo?"* (La moto solo agarra hasta 15 kg, el auto hasta 100 kg, el camión hasta 2000 kg).
6. **En ruta:** El chofer que gana el pedido saca la ruta del **Singleton de Rutas** (870 rutas precalculadas de Arequipa), cambia su estado a `EN_CAMINO` y empieza a avanzar punto por punto.
7. **Reflejo en pantalla:** El modelo no sabe qué botones hay en la pantalla; simplemente grita *"¡El pedido 501 avanzó!"* usando el patrón **Observer**. La vista de Qt escucha ese grito y actualiza el mapa a 60 cuadros por segundo.
8. **Llegada y justicia:** Al llegar, el chofer marca `COMPLETADO`, se lanza un efecto de onda expansiva en el mapa, y el chofer hace una pausa voluntaria de **50 milisegundos**. ¿Por qué? Para que el otro chofer de su misma categoría tenga tiempo de agarrar el siguiente pedido y no se quede uno solo trabajando mientras el otro mira (Round-Robin cooperativo).

---

# 📁 3. Mapa del tesoro: Archivos más importantes

El proyecto tiene 57 archivos, pero estos son los **10 archivos clave** que sostienen todo el sistema y que debes tener en mente:

| Archivo | ¿Qué hace en cristiano? | Capa |
|---|---|:---:|
| `src/model/Pedido.h` | La ficha del paquete (id, origen, destino, peso, estado, costo). | Modelo |
| `src/model/Vehiculo.h` | La clase abstracta padre de `Moto`, `Auto` y `Camion`. Define los métodos virtuales puros. | Modelo |
| `src/model/Cola.h` | Nuestra cola genérica propia `template <typename T>` protegida con cerrojos (`std::mutex`) para que los hilos no choquen. | Genéricos / STL |
| `src/model/SimuladorEntregas.h/.cpp` | El cerebro multihilo. Crea los hilos de los choferes, los duerme, los despierta y controla la velocidad y el tráfico. | Concurrencia |
| `src/model/ServicioRutas.h/.cpp` | El GPS offline (Singleton). Contiene los 30 nodos de Arequipa y las 870 calles de OSRM en memoria. | Patrones GoF |
| `src/model/Observer.h` | La campanita de notificaciones. Define `ISubject` e `IObserver` con punteros débiles (`weak_ptr`). | Patrones GoF |
| `src/controller/PedidoController.h` | El intermediario entre la ventana de pedidos y la base de datos en memoria. | Controlador |
| `src/controller/SimulacionController.h` | El intermediario que recibe tus clics de "Iniciar", "Pausa", "Tráfico" y se los pasa a los hilos. | Controlador |
| `src/view/MainWindow.cpp` | La ventana principal de la aplicación. Une el mapa, las tablas, los menús de archivo (JSON) y el modo oscuro. | Vista |
| `src/view/MapaView.cpp` | El lienzo 2D interactivo (`QGraphicsScene`). Dibuja las calles de Arequipa, los carritos girando en su ángulo real y el Mini-HUD flotante. | Vista |

---

# 🎓 4. Lo referente al curso: Tecnología de Objetos

El profesor evalúa 4 pilares obligatorios. Aquí tienes la explicación exacta de cómo SIGER cumple cada uno:

---

## Pilar A: POO Avanzada
* **Clases Abstractas Puras:** `Vehiculo` y `Paquete` tienen métodos igualados a cero (`= 0`). Por ejemplo:
  ```cpp
  virtual double calcularCosto(double distKm, double pesoKg) const = 0;
  ```
  Esto significa que está **prohibido crear un `Vehiculo` a secas**; estás obligado a crear una `Moto`, un `Auto` o un `Camion`.
* **Polimorfismo Dinámico:** En tiempo de ejecución, el despachador trata a todos como punteros a `Vehiculo*`. Cuando llama a `v->calcularTiempoEntrega()`, C++ busca automáticamente en la tabla virtual (`vtable`) si es una moto (60 km/h) o un camión (40 km/h) y aplica la velocidad correcta.
* **Destructores Virtuales (`virtual ~Vehiculo() = default;`):** Vital para que cuando borres un vehículo mediante su puntero base, se destruya limpiamente la clase hija y no haya fugas de memoria (*slicing*).
* **Encapsulamiento estricto:** Nada de variables públicas. Todo es `protected` o `private`, accesible solo con `getters` marcados con `const` (para prometer que leer el dato no lo modifica) y `[[nodiscard]]` (para que el compilador avise si alguien llama al getter y olvida usar el valor).

---

## Pilar B: Los 5 Patrones de Diseño (GoF) explicados con analogías

Normalmente piden 3 o 4 patrones; **nosotros tenemos 5**:

### 1. Model-View-Controller (MVC) — *Arquitectónico*
* **La analogía:** Un restaurante. El **Modelo** es el chef en la cocina (hace la comida y no sabe quién se la come). La **Vista** es el cliente en la mesa con el menú (mira y pide). El **Controlador** es el mesero (lleva la comanda de la mesa a la cocina y trae el plato terminado).
* **En SIGER:** `GestorPedidos` (Modelo) jamás toca una ventana de Qt. `MainWindow` (Vista) jamás modifica una variable de un pedido directamente. Todo pasa por `PedidoController`.

### 2. Factory Method — *Creacional*
* **La analogía:** Una concesionaria multimarca. Tú no vas a ensamblar una moto pieza por pieza con tuercas en la mano. Le dices a la fábrica: *"Fábrica, dame una moto con placa MTO-501"* y la fábrica te la entrega lista.
* **En SIGER:** `FactoriaVehiculos` tiene registradas `MotoFactory`, `AutoFactory` y `CamionFactory`. Si mañana agregamos drones de entrega, solo creamos `DronFactory` sin tocar ni romper el código viejo (Principio Abierto/Cerrado).

### 3. Strategy — *Comportamiento*
* **La analogía:** El taxímetro con tarifa nocturna vs diurna. El taxista no cambia de auto; cambia el chip del taxímetro según la situación.
* **En SIGER:** La interfaz `IEstrategiaTarifa`. Si el paquete es normal usa `TarifaEstandarStrategy`. Si marcas que es frágil, el sistema le enchufa `TarifaFragilStrategy`, que automáticamente le recarga un 40% al costo por seguro y manipulación delicada.

### 4. Singleton (de Scott Meyers) — *Creacional*
* **La analogía:** La torre de control del aeropuerto o el mapa oficial de la ciudad. No quieres que cada chofer cargue en su guantera una copia de 7 megabytes del mapa de Arequipa porque colapsarías la memoria. Quieres **un solo mapa oficial en toda la empresa**, compartido por todos.
* **En SIGER:** `ServicioRutas::instancia()`. Se inicializa una sola vez de forma thread-safe garantizada por el estándar C++17 y contiene las 870 rutas viales.

### 5. Observer (Publisher-Subscriber) — *Comportamiento*
* **La analogía:** Un canal de YouTube con campanita. Cuando el youtuber sube video, no va casa por casa tocándole la puerta a cada suscriptor. Solo publica el video y la campana le avisa automáticamente a los que están suscritos.
* **En SIGER:** El `SimuladorEntregas` es el sujeto (`ISubject`). `MapaView` y `FlotaView` son los observadores (`IObserver`). Cuando un coche dobla una esquina, el simulador avisa: *"¡Coche 2 se movió!"*. Las ventanas reciben la señal y se redibujan solas.
* **El detalle pro:** Usamos `std::weak_ptr` para que si una ventana se cierra, no se quede pegada en la memoria ni cause errores de puntero colgado.

---

## Pilar C: Programación Genérica y Memoria STL
* **Plantilla `Cola<T>`:** No usamos `std::queue` pelada porque no es segura si dos hilos meten y sacan cosas a la vez. Creamos nuestra propia clase con plantillas `template <typename T>`, envolviéndola con un cerrojo `std::mutex`. Además le inventamos el método `extraerSi(predicado)`, que permite buscar en la cola usando una condición (ej. buscar pedidos menores a 15 kg).
* **Plantilla `Repositorio<T, Key>`:** Un almacén genérico para buscar con funciones lambda.
* **Cero punteros crudos (`new`/`delete`):** Todo se gestiona con punteros inteligentes de la librería estándar (STL):
  * `std::unique_ptr`: Cuando un solo dueño tiene el objeto (ej. las fábricas de vehículos).
  * `std::shared_ptr`: Cuando varios subsistemas comparten el mismo objeto (ej. el pedido que está en la cola, en la tabla y en el mapa al mismo tiempo).
  * `std::weak_ptr`: Para observar sin adueñarse (evita el temido ciclo de referencias donde dos objetos se agarran mutuamente y nunca se destruyen).

---

## Pilar D: Concurrencia y Multihilo
* **Modelo Productor-Consumidor:** 
  * El **Productor** eres tú cuando registras pedidos o presionas el botón de demo.
  * El **Búfer** es nuestra `Cola<Pedido>`.
  * Los **Consumidores** son los hilos de los choferes (`std::thread`).
* **¿Por qué no usamos un bucle infinito `while(true) sleep(1)`?** Porque eso consume 100% de CPU como un ventilador loco. Usamos `std::condition_variable`: el hilo entra en coma profundo en el sistema operativo y no consume ni 0.001% de procesador hasta que alguien hace `m_condVar.notify_all()`.
* **Prevención de Deadlocks (Bloqueos Mutuos):** Siempre soltamos el cerrojo (`lock.unlock()`) **antes** de simular el viaje o avisar al observador. Si un hilo retuviera el cerrojo mientras recorre la ruta, todos los demás hilos se congelarían para siempre.

---

# 🌟 5. TU PARTE FUERTE: Lo que tú vas a exponer (German Chipana)

Como líder de la arquitectura, controlador, GUI y demostración en vivo, a ti te tocan los momentos más importantes y visuales de la sustentación: **la apertura, la arquitectura y el Live Demo**.

---

## A. Tus Diapositivas

### 📌 Slide 1 — Portada y Apertura (1 minuto)
* **Tu mensaje clave:** Seguridad, claridad y orgullo del proyecto.
* **Qué decir:**
  > *"Buenos días profesor, jurado presente. Mi nombre es German Chipana y, junto a mis compañeros Leonardo Baca, Giovanni Mejia y Renato Ponce, tenemos el agrado de presentar **SIGER**: Sistema Integrado de Gestión y Simulación de Entregas Rápidas.*  
  > *Este proyecto no es solo una maqueta visual; es una plataforma construida en C++17 moderno con Qt6 que resuelve un problema real de logística urbana en Arequipa. Implementa 5 patrones de diseño GoF, programación genérica propia, concurrencia multihilo con hilos independientes del sistema operativo y 94 pruebas unitarias automatizadas con 100% de éxito. A continuación, les explicaremos la arquitectura y los fundamentos de objetos que dan vida a SIGER."*

### 📌 Slide 2 — Problemática y Solución (1 minuto)
* **Tu mensaje clave:** Por qué nació este proyecto.
* **Qué decir:**
  > *"En ciudades con la geografía y el tráfico de Arequipa, las empresas de reparto colapsan por asignación manual e ineficiente de pedidos. No hay visibilidad en tiempo real y los pedidos urgentes se retrasan.*  
  > *SIGER soluciona esto con tres pilares:*  
  > *1. Asignación automática por compatibilidad de peso y vehículo.*  
  > *2. Concurrencia real: cada repartidor es un hilo que compite limpiamente.*  
  > *3. Enrutamiento real: no calculamos líneas rectas imaginarias; calculamos giros y distancias reales usando 870 rutas precalculadas de OpenStreetMap (OSRM)."*

### 📌 Slide 3 — Arquitectura MVC y Métricas (1 minuto)
* **Tu mensaje clave:** Desacoplamiento profesional y escala del proyecto.
* **Qué decir:**
  > *"Diseñamos el sistema siguiendo estrictamente el patrón arquitectónico Modelo-Vista-Controlador.*  
  > *El **Modelo** contiene la lógica pura: el Gestor de Pedidos, el Simulador y nuestras estructuras genéricas. No tiene una sola línea de código gráfico.*  
  > *Las **Vistas** son componentes de Qt6 como el mapa 2D, el monitoreo de flota y las estadísticas.*  
  > *Y los **Controladores** son el puente: reciben los eventos de la interfaz y mutan el modelo mediante señales y slots.*  
  > *En números: el proyecto cuenta con 57 archivos de código fuente, más de 5,300 líneas efectivas de C++, 30 nodos emblemáticos de Arequipa y 870 rutas viales completas."*

---

## B. TU MOMENTO ESTRELLA: El Live Demo (Slide 10)

Aquí es donde te ganas el 20 del profesor demostrando la aplicación en vivo. Sigue esta **secuencia exacta**:

```
[Paso 1: Lanzar App] ──> [Paso 2: Lote Demo 10] ──> [Paso 3: Tráfico Hora Punta]
                              │
[Paso 6: Modo Oscuro Ctrl+D] <── [Paso 5: Cockpit F10] <── [Paso 4: Iniciar Simulación]
       │
[Paso 7: Ver Flota y HUD] ──> [Paso 8: Estadísticas] ──> [Paso 9: Guardar JSON Ctrl+S]
```

### El libreto exacto que debes decir mientras operas la aplicación:

1. **Abrir la app:**
   > *"Como pueden observar en pantalla, esta es la interfaz principal de SIGER. En el centro tenemos el mapa de Arequipa con 30 nodos reconocibles: la Plaza de Armas, el Mirador de Yanahuara, la UNSA, el Mall Aventura, entre otros."*

2. **Presionar el botón `🎲 Lote Demo...` (Selecciona 10 pedidos):**
   > *"Para no perder tiempo llenando formularios manualmente en esta sustentación, implementamos un generador de lotes demo. Con un solo clic, el sistema genera 10 pedidos realistas en Arequipa con diferentes pesos y tipos de carga."*  
   *(Muestra cómo se llena la tabla de pedidos abajo y cómo las tarifas ya se calcularon con el patrón Strategy).*

3. **Cambiar la condición de Tráfico a "Hora Punta (1.6x)":**
   > *"Además, implementamos simulación de tráfico. Voy a colocar la condición en 'Hora Punta (1.6x)', lo que aumentará cinemáticamente el tiempo de viaje de los vehículos en un 60% debido a la congestión de las avenidas."*

4. **Hacer clic en `▶ Iniciar Simulación`:**
   > *"Al hacer clic en Iniciar Simulación, se despiertan los hilos en segundo plano. Observen cómo las motos salen primero por ser más rápidas, seguidas por los autos y el camión. Cada vehículo sigue el trazado exacto de las avenidas reales de Arequipa."*  
   *(Señala el Mini-HUD flotante en la esquina superior izquierda del mapa con el cronómetro digital y el conteo de pedidos en tránsito).*

5. **Presionar la tecla `F10` (Cockpit Unificado):**
   > *"Para evitar la fatiga de estar cambiando de pestaña en plena operación logística, diseñamos el 'Cockpit Unificado' con la tecla F10. Esto divide la pantalla con un QSplitter interactivo, permitiendo supervisar el mapa y la tabla de pedidos al mismo tiempo."*

6. **Presionar `Ctrl+D` (Modo Oscuro):**
   > *"Si los operadores trabajan en turno noche, con Ctrl+D activamos el Modo Oscuro de alta tecnología, adaptando la paleta de colores a tonos grafito y neón para reducir la fatiga visual."*

7. **Ir a la pestaña "Monitoreo de Flota":**
   > *"En esta pestaña podemos ver las tarjetas individuales de cada chofer: Carlos y Lucía en las Motos, María y Jorge en los Autos, y Pedro en el Camión. Cada uno tiene su barra de progreso y su conteo de pedidos completados en tiempo real."*

8. **Esperar que un repartidor llegue al destino:**
   > *"Miren el mapa en este momento: cuando un conductor llega a su destino, se dispara una animación de pulso u onda expansiva en las coordenadas exactas de entrega, celebrando el éxito del despacho."*

9. **Ir a la pestaña "Estadísticas":**
   > *"Aquí tenemos el consolidado financiero: pedidos entregados, facturación total en Soles acumulada y el gráfico circular interactivo generado con QtCharts."*

10. **Presionar `Ctrl+S` (Persistencia JSON):**
    > *"Finalmente, el sistema cuenta con persistencia. Si presiono Ctrl+S, puedo guardar toda la sesión activa en un archivo JSON estructurado para cargarla en cualquier momento con Ctrl+O sin perder información."*

---

# 🛡️ 6. Banco de Preguntas Tramposas del Jurado (¡Y cómo destruirlas!)

Si el jurado te interrumpe o te hace preguntas difíciles, aquí tienes las respuestas directas:

### 1. ¿Por qué crearon su propia clase `Cola<T>` en vez de usar `std::queue` o `QQueue`?
* **Tu respuesta:**  
  > *"Porque `std::queue` y `QQueue` no son thread-safe por defecto; si dos hilos intentan hacer push y pop a la vez, el programa colapsa por condición de carrera. Nuestra plantilla `Cola<T>` encapsula un `std::mutex` con `std::lock_guard` en cada operación. Además, le agregamos el método `extraerSi(predicado)` que permite buscar pedidos condicionalmente (por ejemplo, pedidos que pesen menos de 15 kg para la moto) sin desordenar la cola FIFO."*

### 2. ¿Cómo se comunican el Modelo y la Vista sin acoplarse?
* **Tu respuesta:**  
  > *"A través de dos mecanismos: dentro del dominio puro usamos el patrón **Observer (GoF)**, donde el modelo notifica eventos con `ISubject` e `IObserver` usando `std::weak_ptr`. Y en la capa de interfaz, usamos el sistema nativo de **Signals y Slots de Qt**, donde los Controladores emiten señales que las ventanas escuchan de forma asíncrona."*

### 3. ¿Por qué usaron `weak_ptr` en el Observer y no `shared_ptr`?
* **Tu respuesta:**  
  > *"Para evitar referencias circulares y fugas de memoria. Si el Sujeto tuviera un `shared_ptr` a la Vista y la Vista tuviera un `shared_ptr` al Sujeto, el contador de referencias nunca llegaría a cero y la memoria jamás se liberaría al cerrar la ventana. Con `weak_ptr`, la referencia es débil; y si una vista se destruye, el sujeto simplemente purga el puntero expirado en la siguiente notificación."*

### 4. ¿De dónde salieron las rutas de Arequipa? ¿Están consultando internet en vivo?
* **Tu respuesta:**  
  > *"No consultamos internet en vivo para evitar lag o fallas si se cae la red durante la simulación. Hicimos un preprocesamiento con el motor OSRM (Open Source Routing Machine) y OpenStreetMap sobre los 30 nodos de Arequipa. Calculamos las 870 combinaciones posibles de rutas calle por calle y las guardamos en una caché local JSON de 6.89 MB. El Singleton `ServicioRutas` la carga en memoria en milisegundos al arrancar el programa."*

### 5. ¿Qué pasa si la ventana se cierra mientras los hilos están corriendo? ¿Hay riesgo de Crash o Deadlock?
* **Tu respuesta:**  
  > *"No, está totalmente protegido. Al cerrar la ventana o presionar Detener, el destructor activa la bandera atómica `m_corriendo = false`, ejecuta `m_condVar.notify_all()` para sacar a todos los hilos de su estado de espera, y ejecuta `hilo.join()` estrictamente **fuera del mutex**. Esto garantiza una parada limpia sin deadlocks ni fugas de recursos."*

### 6. ¿Por qué tienen 94 pruebas en CTest? ¿Qué prueban?
* **Tu respuesta:**  
  > *"Tenemos 3 suites automatizadas:*  
  > *1. `test_dominio` (41 pruebas): Polimorfismo de vehículos, cálculo de tarifas de Strategy y restricciones de peso.*  
  > *2. `test_generico` (39 pruebas): Comportamiento de plantillas `Cola`, `Repositorio`, precisión de `Ruta` y unicidad del Singleton.*  
  > *3. `test_threading` (14 pruebas): Pruebas de estrés donde inyectamos 9 pedidos en simultáneo y verificamos que 5 hilos los completan sin perder ningún paquete y sin condiciones de carrera.*  
  > *Las 94 pasan al 100% en menos de 2.5 segundos."*

---

# 🚀 7. Resumen de comandos útiles para el día D

Ten estos comandos a mano en tu terminal de PowerShell por si el jurado te pide compilar o correr pruebas:

* **Compilar el proyecto:**
  ```powershell
  cmake --build build
  ```
* **Correr las pruebas unitarias automatizadas (¡Queda genial mostrar esto!):**
  ```powershell
  ctest --test-dir build --output-on-failure
  ```
* **Lanzar la aplicación:**
  ```powershell
  ./build/SIGER.exe
  ```
* **Abrir la presentación de diapositivas:**
  ```powershell
  Start-Process "docs/presentacion/presentacion_siger.html"
  ```
* **Abrir el PDF del paper IEEE:**
  ```powershell
  Start-Process "docs/ieee/SIGER_Paper_IEEE.pdf"
  ```

---

### 💡 Consejo final para German:
Tienes en tus manos un proyecto extraordinario, completo y técnicamente impecable. Entiende el flujo, mantén la calma y habla con la naturalidad de quien conoce su creación. **¡A por ese 20!** 🎓
