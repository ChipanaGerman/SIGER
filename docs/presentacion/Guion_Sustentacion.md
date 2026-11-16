# 🎤 Guión de Sustentación — SIGER
**Sistema Integrado de Gestión y Simulación de Entregas Rápidas**  
**Curso:** Tecnología de Objetos · UNSA — Arequipa, 2026

> [!TIP]
> Tecla **`S`** en la presentación para abrir la **Vista de Presentador** con el guión y el cronómetro.  
> Tecla **`F`** para modo pantalla completa. **`ESC`** para salir del lightbox de imágenes.

---

## 📋 Reparto de Exposición

| Slide | Contenido | Expositor | Tiempo |
|:---:|:---|:---|:---:|
| 01 | Portada y presentación del equipo | **German Arturo Chipana Jeronimo** | ~1 min |
| 02 | Problemática y solución SIGER | **German Arturo Chipana Jeronimo** | ~1 min |
| 03 | Arquitectura MVC y stack tecnológico | **German Arturo Chipana Jeronimo** | ~1 min |
| 04 | POO Avanzada — Jerarquías polimórficas | **Leonardo Juan Jose Baca Calsin** | ~1 min |
| 05 | Patrones GoF I: Factory Method & Strategy | **Leonardo Juan Jose Baca Calsin** | ~1.5 min |
| 06 | Programación genérica, Templates y RAII | **Giovanni Patrick Mejia Rondan** | ~1.5 min |
| 07 | Patrones GoF II: Singleton & Observer | **Giovanni Patrick Mejia Rondan** | ~1.5 min |
| 08 | Concurrencia — Modelo Productor/Consumidor | **Renato Xavier Ponce Llerena** | ~1.5 min |
| 09 | Balanceo de carga y prevención de Deadlocks | **Renato Xavier Ponce Llerena** | ~1 min |
| 10 | Live Demo — Cockpit Dashboard | **German Arturo Chipana Jeronimo** | ~2 min |
| 11 | Pruebas unitarias y control de calidad | **Todos** | ~1 min |
| 12 | Conclusiones y preguntas | **Todos** | ~1 min |

**⏱ Tiempo total estimado: 15 minutos (exposición + 3-5 min de preguntas)**

---

## 🎙️ Guión Detallado por Integrante

---

### SLIDE 01 — Portada
**Expositor: German Arturo Chipana Jeronimo**

> *"Buenos días / tardes, profesor. Somos el equipo SIGER de la Escuela Profesional de Ingeniería de Sistemas de la UNSA. Presento a mis compañeros: Leonardo Baca, Giovanni Mejia y Renato Ponce.*
>
> *Hoy les presentamos **SIGER**: un Sistema Integrado de Gestión y Simulación de Entregas Rápidas, desarrollado en C++17 con Qt6, que combina concurrencia multihilo real, cinco patrones de diseño GoF, plantillas genéricas propias, y enrutamiento por calles reales de Arequipa usando datos viales de OSRM.*
>
> *El proyecto supera los 5,300 líneas de código efectivo, implementa 94 aserciones de pruebas unitarias con tasa de aprobación del 100%, y resuelve un problema logístico real de última milla en nuestra ciudad."*

---

### SLIDE 02 — Problemática y Solución
**Expositor: German Arturo Chipana Jeronimo**

> *"El problema que abordamos es la gestión ineficiente de pedidos simultáneos en empresas de delivery urbano en Arequipa.*
>
> *Sin herramientas especializadas, la asignación de repartidores es manual, no hay visibilidad del estado de la flota en tiempo real, y los pedidos urgentes compiten sin priorización con los pedidos normales.*
>
> *SIGER resuelve esto con tres pilares: primero, **asignación automática** basada en tipo de vehículo y peso del paquete. Segundo, **simulación concurrente real** donde cada repartidor corre en su propio hilo del sistema operativo. Y tercero, **rutas por calles reales** de Arequipa descargadas del motor de ruteo OSRM y almacenadas en caché local offline."*

---

### SLIDE 03 — Arquitectura MVC
**Expositor: German Arturo Chipana Jeronimo**

> *"La arquitectura del sistema sigue el patrón MVC de forma estricta.*
>
> *El **Modelo** concentra toda la lógica de negocio: GestorPedidos, SimuladorEntregas y nuestras estructuras de datos genéricas.*
>
> *Los **Controladores** PedidoController y SimulacionController actúan como intermediarios: traducen las acciones de la vista en llamadas al modelo y propagan los eventos del modelo hacia la vista usando el sistema de señales y slots de Qt.*
>
> *Las **Vistas** renderizan el estado sin conocer ningún detalle de la implementación interna. MainWindow, MapaView, FlotaView y EstadisticasView solo saben que reciben señales con datos actualizados.*
>
> *En términos de escala: 57 archivos fuente, 5333 líneas de código, 30 nodos metropolitanos de Arequipa y 870 rutas viales precalculadas."*

---

### SLIDE 04 — POO Avanzada
**Expositor: Leonardo Juan Jose Baca Calsin**

> *"Me corresponde presentar la capa de dominio del sistema.*
>
> *Implementamos dos jerarquías polimórficas rigurosas. La clase abstracta **Vehiculo** declara tres métodos virtuales puros: calcularCosto, getTipo y calcularTiempoEntrega. Las subclases Moto, Auto y Camión tienen implementaciones completamente distintas: una Moto puede cargar hasta 15 kg y viaja a 60 km/h; un Camión puede cargar 2000 kg pero a 40 km/h.*
>
> *Usamos los especificadores modernos de C++17: el destructor virtual garantiza que nunca haya slicing ni fugas de memoria. El atributo [[nodiscard]] obliga al compilador a generar una advertencia si el resultado de un getter se descarta accidentalmente. Y el = delete en constructores de copia impide que el compilador genere operaciones peligrosas.*
>
> *Todo el encapsulamiento es estricto: los atributos son privados y solo accesibles mediante getters calificados con const."*

---

### SLIDE 05 — Factory Method & Strategy
**Expositor: Leonardo Juan Jose Baca Calsin**

> *"Los primeros dos patrones GoF que implementamos son Factory Method y Strategy.*
>
> ***Factory Method**: la clase abstracta VehiculoFactory declara un método crear que devuelve un unique_ptr a Vehiculo. Cada fábrica concreta sabe cómo construir su vehículo. El registro de fábricas actúa como un mapa de constructores por tipo: si el sistema necesita una Moto, busca 'moto' en el mapa y obtiene un MotoFactory listo para usar.*
>
> *Esto desacopla completamente la creación de objetos de su uso. El SimuladorEntregas no sabe ni le importa si está creando una Moto o un Camión; simplemente pide al factory el tipo que necesita.*
>
> ***Strategy**: la interfaz IEstrategiaTarifa tiene un único método calcular. Tenemos tres implementaciones concretas: TarifaEstandarStrategy, TarifaFragilStrategy que añade un 40% por manejo especial, y TarifaPesadaStrategy para paquetes de más de 50 kg. La tarifa se calcula en tiempo real mientras el operador llena el formulario de registro de pedidos."*

---

### SLIDE 06 — Programación Genérica
**Expositor: Giovanni Patrick Mejia Rondan**

> *"Mi responsabilidad fue el módulo de programación genérica y gestión de memoria.*
>
> *Nuestra estructura de datos central es **Cola T**: un contenedor FIFO genérico completamente thread-safe. Cada operación —push, pop, peek— adquiere un lock_guard sobre el mutex interno antes de acceder a la cola subyacente. El método extraerSi es el más interesante: recibe un predicado lambda y extrae solo el primer elemento que lo satisface, reconstruyendo la cola con el orden original para los demás elementos. Esto permite que una Moto extraiga solo pedidos de menos de 15 kg sin afectar los pedidos de Camión.*
>
> *Usamos los tres tipos de punteros inteligentes de la STL con propósitos distintos: unique_ptr para propiedad exclusiva en las factorías; shared_ptr para los pedidos que viajan entre el modelo y la vista; y weak_ptr en la lista de observadores para evitar ciclos de referencia que causarían fugas de memoria.*
>
> *El resultado es cero llamadas a new y delete manuales en toda la lógica de negocio."*

---

### SLIDE 07 — Singleton & Observer
**Expositor: Giovanni Patrick Mejia Rondan**

> *"Los otros dos patrones GoF son Singleton y Observer.*
>
> ***Singleton**: ServicioRutas es el motor de ruteo vial. Debe haber exactamente una instancia porque carga en memoria 870 rutas de Arequipa con sus coordenadas GPS completas, lo que toma varios segundos y ocupa varios megabytes. El patrón Meyer's que usamos aprovecha la garantía de C++17: las variables estáticas locales se inicializan exactamente una vez, de forma thread-safe, sin necesidad de mutex adicional.*
>
> ***Observer**: el ISubject gestiona una lista de weak_ptr a IObserver. Cuando el modelo cambia de estado —por ejemplo, un pedido pasa de NUEVO a EN_CAMINO— llama a notificarObservers con el payload DatoEvento. Las vistas reciben ese evento y se actualizan. Crítico: la notificación ocurre fuera del mutex de la lista de observers. Esto previene un deadlock anidado si un observador intenta suscribirse durante su propio callback."*

---

### SLIDE 08 — Concurrencia
**Expositor: Renato Xavier Ponce Llerena**

> *"El módulo de concurrencia es el corazón del sistema.*
>
> *Implementamos el modelo clásico Productor-Consumidor. El Productor puede ser el formulario de registro manual o el Lote Demo que genera pedidos automáticamente. Los pedidos se insertan en la Cola T compartida. Del otro lado tenemos hasta cinco Consumidores, uno por repartidor, cada uno corriendo en su propio hilo del sistema operativo.*
>
> *El flujo de cada hilo-consumidor es: adquirir el unique_lock sobre el mutex del simulador; llamar a condition_variable.wait con un predicado que espera hasta que la cola no esté vacía o el sistema se detenga; si hay pedidos, llamar a extraerSi con el predicado de tipo de vehículo; liberar el lock inmediatamente; y ejecutar la entrega sin el lock para maximizar el paralelismo.*
>
> *El notify_all en el productor despierta a todos los hilos simultáneamente, y cada uno compite de forma segura por el primer pedido que sea compatible con su vehículo."*

---

### SLIDE 09 — Balanceo y Deadlocks
**Expositor: Renato Xavier Ponce Llerena**

> *"Dos problemas clásicos de concurrencia que resolvemos: inanición y deadlocks.*
>
> *Para el **balanceo de carga**: sin mecanismo adicional, el primer hilo que se despierta atendería todos los pedidos de forma secuencial, dejando al segundo conductor del mismo tipo completamente inactivo. Nuestra solución es un Round-Robin cooperativo voluntario: después de completar una entrega, el hilo cede el turno durante 50 milisegundos. Ese tiempo es suficiente para que el scheduler del sistema operativo active al otro hilo del mismo tipo.*
>
> *Para la **prevención de deadlocks**: seguimos cuatro reglas estrictas. Primero, siempre liberamos el lock antes de llamar a simularEntrega. Segundo, las notificaciones al Observer ocurren fuera del mutex de la lista. Tercero, los thread::join se llaman fuera del lock de simulación. Y cuarto, el despacho escalonado de 700 milisegundos evita que dos vehículos salgan en el mismo instante y generen condiciones de carrera en la animación del mapa.*
>
> *Estas reglas están verificadas por las 14 aserciones de test_threading."*

---

### SLIDE 10 — Live Demo
**Expositor: German Arturo Chipana Jeronimo**

**Secuencia de demostración (seguir en orden):**

1. Lanzar `build/SIGER.exe` si no está abierto.
2. Presionar **`🎲 Lote Demo`** → seleccionar **10 pedidos**.
3. Cambiar el selector de tráfico a **"Hora Punta (1.6x)"**.
4. Presionar **`▶ Iniciar Simulación`**.
5. Señalar el Mini-HUD flotante: *"Telemetría en tiempo real: pedidos en cola, en tránsito, factor de tráfico y cronómetro."*
6. Presionar **`F10`** para ver el Cockpit Unificado (mapa + tabla simultáneos).
7. Presionar **`Ctrl+D`** para activar el Modo Oscuro.
8. Ir a pestaña **"Monitoreo de Flota"** para mostrar las 5 tarjetas en paralelo.
9. Esperar que al menos un repartidor llegue a su destino y señalar el efecto de pulso/onda expansiva.
10. Ir a pestaña **"Estadísticas"** para mostrar los KPIs y el gráfico circular.
11. Presionar **`Ctrl+S`** para mostrar el guardado de sesión en JSON.

> *"Todo lo que están viendo ocurre con 5 hilos reales del sistema operativo corriendo en paralelo, coordinados por primitivas de sincronización de C++ estándar."*

---

### SLIDE 11 — Pruebas Unitarias
**Todos los integrantes**

> *"Para garantizar la corrección de cada capa del sistema, implementamos tres suites de pruebas automatizadas con el framework CTest de CMake.*
>
> *test_dominio: 41 aserciones que verifican polimorfismo, encapsulamiento, restricciones de capacidad y el cálculo correcto de tarifas para cada estrategia.*
>
> *test_generico: 39 aserciones sobre nuestros templates: que Cola T sea FIFO y thread-safe, que Repositorio encuentre elementos con predicados lambda, que Ruta calcule distancias correctamente, y que el Singleton devuelva siempre la misma instancia.*
>
> *test_threading: 14 aserciones de estrés donde lanzamos 9 pedidos simultáneos con 5 repartidores y verificamos que todos se completan, que ningún pedido se pierde y que la distribución es razonablemente equilibrada.*
>
> *Resultado: 94 de 94 aserciones aprobadas. Tiempo total: 2.43 segundos."*

---

### SLIDE 12 — Conclusiones
**Todos los integrantes**

> *"Para concluir: SIGER cumple y supera todos los requerimientos académicos del curso de Tecnología de Objetos.*
>
> *En POO avanzada: dos jerarquías abstractas con polimorfismo dinámico y especificadores modernos de C++17. En patrones GoF: cinco patrones implementados y verificados. En programación genérica: tres templates propios con cero fugas de memoria. En concurrencia: modelo productor-consumidor con sincronización completa y sin deadlocks verificados por pruebas automatizadas. Y en interfaz gráfica: un cockpit visual con enrutamiento real por calles de Arequipa.*
>
> *El proyecto está publicado en GitHub en github.com/ChipanaGerman/SIGER con historial de commits y autoría de todo el equipo.*
>
> *Estamos abiertos a las preguntas del jurado."*

---

## ❓ Posibles Preguntas del Jurado y Respuestas Preparadas

| Pregunta | Respuesta |
|:---|:---|
| *"¿Por qué usaron weak_ptr en el Observer?"* | Porque si una Vista se destruye mientras el Modelo sigue vivo, el weak_ptr expira silenciosamente y se purga en la siguiente notificación. Con shared_ptr habría un ciclo Modelo↔Vista que impediría la destrucción de ambos. |
| *"¿Cómo garantizan que no hay race conditions en la Cola?"* | Cada operación de Cola T —push, pop, peek, extraerSi— adquiere un lock_guard exclusivo sobre su mutex interno. lock_guard es RAII: libera el lock automáticamente al salir del scope, incluso si hay una excepción. |
| *"¿Qué pasa si dos repartidores intentan tomar el mismo pedido?"* | Es imposible por diseño. extraerSi adquiere el mutex antes de inspeccionar la cola y lo mantiene durante toda la extracción. El segundo hilo que llega al lock espera hasta que el primero termine. |
| *"¿Por qué usaron condition_variable en vez de polling?"* | El polling con sleep en bucle desperdiciaría CPU constantemente. condition_variable pone el hilo en estado bloqueado a nivel del kernel hasta que llega la notificación, con consumo de CPU cercano a cero mientras espera. |
| *"¿Cómo se obtienen las rutas de Arequipa?"* | Se obtuvieron offline desde el motor OSRM consultando los 870 pares origen-destino entre los 30 nodos metropolitanos. Las respuestas se almacenaron en un archivo JSON de 6.89 MB que se carga al inicio. En tiempo de ejecución no hay conexión a Internet. |
| *"¿El Singleton es thread-safe?"* | Sí. El estándar C++11 garantiza que la inicialización de variables estáticas locales es thread-safe. La instancia se inicializa exactamente una vez incluso si múltiples hilos la solicitan simultáneamente. |
| *"¿Qué es el Modo Demo?"* | Un generador de lote aleatorio que crea 5 o 10 pedidos con orígenes, destinos, pesos y descripciones coherentes para Arequipa en un solo clic, sin necesidad de llenar el formulario manualmente. |

---

## 🗓️ Preparación Final — Checklist

- [ ] `SIGER.exe` arranca correctamente desde `build/SIGER.exe`
- [ ] Las capturas en `img/` se muestran en las slides (rutas relativas `../../img/`)
- [ ] La presentación abre en el navegador sin errores de consola
- [ ] Tecla `S` activa la Vista de Presentador con el guión
- [ ] Tecla `F10` alterna Cockpit Unificado en SIGER
- [ ] `Ctrl+D` activa/desactiva Modo Oscuro en SIGER
- [ ] `ctest --test-dir build --output-on-failure` muestra 100% antes de la sala
- [ ] Cada integrante conoce su slide y guión de memoria
