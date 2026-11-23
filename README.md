# Programación Concurrente y Distribuida — Equipo 5

Trabajo en equipo de los Temas 3 a 8 (hilos, carreras, exclusión mutua con RAII, semáforos, monitores, interbloqueo e inanición).

**Duración:** 2 semanas · **Entrega:** este repositorio con la etiqueta `final` · **Evaluación:** defensa en clase

## Equipo

- Ange Lucas Benoit
- Álvaro Rovira de las Heras
- Alejandro Castro Espín
- Víctor Díez Sánchez
- Vicent David Burgal

## Caso: 5 — Almacén robotizado

> Pendiente de confirmar que al Equipo 5 le corresponde el Caso 5.

Un pasillo circular de 12 celdas donde robots recogen productos de las estanterías y los llevan a empaquetar.

- **Hilos:** Robot, Generador de pedidos, Empaquetador y Gestor de inventario.
- **Clases:** `Robot`, `Pasillo`, `CentroPedidos` (más `Semaforo`).
- **Reglas:**
  1. Pasillo y cargadores: una celda = un robot como mucho; cargadores como monitor, por orden de llegada.
  2. Pedidos e inventario: existencias con `std::shared_mutex`; cola de pedidos con `Semaforo` propio + `std::counting_semaphore` + mutex.
  3. Empaquetado: buffer como monitor, en orden FIFO.
- **Modos de fallo:** `carrera`, `interbloqueo`, `livelock` e `inanicion`.
- **Experimento:** `--robots 4`, `8` y `12` → entregas por segundo.

## Qué hay que hacer

1. Partir del **código base** del profesor (sin modificar `base/`) y compilar sin avisos (`-Wall -Wextra -Wpedantic`) en g++, clang y Visual Studio.
2. Implementar la clase **`Semaforo`** como monitor (`acquire`, `release`, `tryAcquireFor`) y su prueba `prueba_semaforo`.
3. Implementar el **caso**: las 3 clases, los hilos, las reglas, los invariantes y el cierre.
   - Sin variables globales, siempre RAII para los cerrojos, sin espera activa y sin `sleep` para arreglar sincronización.
4. Implementar los **modos de fallo** (`--fallo <modo>`).
5. **Competición:** paralelizar `competicion/Producto.cpp` (producto módulo 1 000 000 007 de `valor(0..N-1)`, con N = 2 000 000).
6. **Documentación:**
   - `DISENO.md`: tabla de predicados de cada monitor.
   - `EXPERIMENTOS.md`: predicción del experimento (en un commit **anterior** al resultado), resultados, salidas de `--estres 10` y tiempos de la competición con 1, 2, 4 y 8 hilos.
   - `IA.md`: registro completo de todo uso de herramientas de IA.
   - `README.md`: cómo ejecutar el caso y con cuántos hilos se mide la competición.

## Reparto

Ordenado de la parte más compleja a la menos compleja.

| # | Parte | Responsable | Incluye |
|---|---|---|---|
| 1 | Pasillo y movimiento | Ange Lucas Benoit | Celdas del pasillo, `Robot::ejecutar()`, modos `interbloqueo` y `livelock` |
| 2 | Pedidos e inventario | Víctor Díez Sánchez | Existencias (`shared_mutex`), cola de pedidos con semáforos, generadores, gestor, modo `carrera` |
| 3 | Cargadores | Alejandro Castro Espín | Monitor de cargadores FIFO, batería, modo `inanicion`, experimento |
| 4 | Semaforo y `main` | Álvaro Rovira de las Heras | `Semaforo`, `prueba_semaforo`, `main.cpp` (cierre, invariantes finales, `Lanzador`), salidas de `--estres 10` |
| 5 | Empaquetado | Vicent David Burgal | Buffer de empaquetado (monitor FIFO), empaquetadores, competición (`Producto.cpp`) |

Todos: la fila de su monitor en `DISENO.md` antes de programarlo, sus consultas en `IA.md` y sus propios commits. Antes de empezar, acordar entre todos los métodos públicos de `Robot`, `Pasillo` y `CentroPedidos`.

## Qué tiene que hacer cada uno

### 1. Ange: Pasillo y movimiento

**Dónde:** `Pasillo` (las celdas) y `Robot` (todo el hilo, `Robot::ejecutar()`).

- **Pasillo:** 12 celdas en círculo (0…11). Cada celda la ocupa como mucho un robot. Para avanzar de `c` a `(c+1) mod 12`, el robot ocupa la celda siguiente **antes** de soltar la actual.
- **Robot:** empieza fuera del pasillo con la batería a 30 y repite este ciclo:
  1. Saca un pedido de la cola. Si es el pedido especial de fin, termina.
  2. Entra por la celda 0 cuando está libre.
  3. Da la vuelta completa de 12 movimientos (1 ms cada uno), pasando por la celda del producto (`2p+1`).
  4. Deja el producto en el buffer de empaquetado. Si está lleno, espera **ocupando la celda 0**. Después sale del pasillo.
  5. Si le quedan menos de 12 movimientos de batería, carga (método de la parte 3).
- **Sin interbloqueo:** el pasillo es circular. Si se llena, todos esperan a la celda de delante y se bloquea todo, así que hay que limitar cuántos robots entran a la vez y justificar ese límite.
- **Invariante:** nunca hay dos robots en la misma celda.
- **Medir:** espera `robot (celda siguiente)`, métricas `movimientos` y `entregas`. Llamar a `Vigilante::progreso()` en cada entrega.
- **Modos de fallo:**
  - `interbloqueo`: sin límite de robots en el pasillo y esperando sin plazo a la celda siguiente.
  - `livelock`: si la siguiente está ocupada, retroceder a la anterior (si está libre), esperar 2 ms y reintentar.
- **Hito 1:** `Robot` y `Pasillo` con los hilos de los robots creados, el ciclo completo y terminando bien, todavía sin sincronizar.

### 2. Víctor: Pedidos e inventario

**Dónde:** `CentroPedidos` (inventario, cola de pedidos, hilos generador y gestor).

- **Inventario:** 6 productos, cada uno empieza con `existencias` unidades. Se protege con `std::shared_mutex`: consultar es lectura (`shared_lock`) y reservar y reponer son escrituras (`unique_lock`). Nunca se reserva una unidad que no existe, así que comprobar y reservar van en la **misma** sección crítica.
- **Generador g:** para cada pedido `k` (0 … `pedidos`-1), el producto es `(7g + k) mod 6`. Si hay existencias, reserva una y mete el pedido en la cola (si está llena, espera). Si no hay, espera 1 ms y pasa al siguiente. Métrica `pedidos creados`.
- **Cola de pedidos** (capacidad 8), hecha con semáforos: los huecos con un `Semaforo` propio, los elementos con `std::counting_semaphore` y un mutex para la cola. Sale en orden FIFO.
- **Pedidos especiales de fin:** un método para que `main` meta uno por robot cuando terminan los generadores.
- **Gestor de inventario:** cada 5 ms repone 1 unidad de cada producto. Termina cuando `main` le avisa, así que tiene que poder despertarse al momento: `wait_for` sobre una variable de condición, no un `sleep` en bucle.
- **Invariante:** existencias ≥ 0.
- **Medir:** espera `robot (pedido)` al sacar de la cola (acordarlo con Ange).
- **Modo de fallo `carrera`:** el generador comprueba las existencias en una sección crítica y reserva en otra distinta.
- **Hito 1:** generadores y gestor creados y terminando, cola funcionando y **modo `carrera` entregado** (es obligatorio para el hito 1).

### 3. Alejandro: Cargadores

**Dónde:** `Pasillo` (la parte de los cargadores). La batería está en `Robot`, así que hay que coordinarse con Ange.

- **Cargadores:** monitor con `cargadores` (P) plazas. Cargar es esperar una plaza libre, mantenerla 5 ms y poner la batería a 30.
- **Por orden de llegada:** los robots cargan en el orden en que llegan. Un `wait` normal no garantiza el orden, hace falta un sistema de turnos (tickets).
- **Medir:** espera `robot (cargador)`.
- **Modo de fallo `inanicion`:** la plaza libre se da siempre al robot de menor número que esté esperando. Se ve con `--cargadores 1 --pedidos 100 --existencias 30`, en la espera máxima del cargador.
- **Experimento** en `EXPERIMENTOS.md`: `--robots 4`, `8` y `12` → entregas por segundo.
  1. Escribir la **predicción razonada** y hacer commit (para el hito 1).
  2. Ejecutar y escribir el resultado real en un commit **posterior**.
- **Hito 1:** el monitor de cargadores con la interfaz acordada y la predicción del experimento escrita.

### 4. Álvaro: Semaforo y `main`

**Dónde:** `semaforo/Semaforo.cpp`, `prueba_semaforo/main.cpp`, `caso/main.cpp` y `caso/CMakeLists.txt`.

- **`Semaforo`:** monitor (mutex + variable de condición) con `acquire()`, `release()` y `bool tryAcquireFor(std::chrono::milliseconds)`. La interfaz de `Semaforo.h` está fijada. **Es lo más urgente**, porque las partes 2 y 3 lo necesitan.
- **`prueba_semaforo`:** 12 hilos que hacen 20 veces *acquire → 300 µs dentro → release* sobre un semáforo de valor 3, primero con `Semaforo` y después con `std::counting_semaphore<3>`.
  - Comprobar con los dos que nunca hay más de 3 hilos dentro.
  - Comprobar que, con valor 0, `tryAcquireFor(50 ms)` / `try_acquire_for(50 ms)` devuelve `false`.
  - Imprimir `OK` o la comprobación que falla.
- **`caso/CMakeLists.txt`:** el nombre del ejecutable y los `.h`/`.cpp` de las 3 clases.
- **`caso/main.cpp`** (seguir la estructura de `ejemplo/main.cpp`):
  - Solo crea los objetos y lanza los hilos (robots, generadores, empaquetadores y gestor).
  - Cierre en este orden: cuando terminan los generadores → un pedido de fin por robot. Cuando terminan los robots → cerrar el buffer de empaquetado. Cuando terminan los empaquetadores → avisar al gestor.
  - Invariante final: entregas = paquetes = pedidos creados.
  - `Lanzador` con las 6 claves y los 4 modos de fallo.
- **`EXPERIMENTOS.md`:** salida de `--estres 10` sin fallo y con cada `--fallo <modo>`, y explicar por qué la misma configuración falla unas veces sí y otras no.
- **Hito 1:** `Semaforo` con `prueba_semaforo` imprimiendo `OK`, y `main` lanzando un hilo de cada tipo y haciendo `join` de todos.

### 5. Vicent: Empaquetado y competición

**Dónde:** `CentroPedidos` (buffer de empaquetado y empaquetadores) y `competicion/Producto.cpp`.

- **Buffer de empaquetado** (capacidad 4): monitor (mutex + variable de condición) en orden FIFO. El robot deja el producto y espera si está lleno. El empaquetador saca uno y espera si está vacío.
- **Empaquetador:** saca un producto y lo empaqueta (2 ms). Métrica `paquetes`, y `Vigilante::progreso()` en cada paquete.
- **Cierre:** `cerrar()` pone una bandera y hace `notify_all`. El empaquetador termina cuando el buffer está cerrado **y** vacío.
- **Competición:** en `Producto.cpp`, `std::uint64_t producto(int hilos)` calcula el producto módulo 1 000 000 007 de `valor(0)` … `valor(N-1)`, con N = 2 000 000.
  - Llamar a `valor(i)` exactamente una vez por cada `i`.
  - Solo `std::thread`, mutex, variables de condición y semáforos.
  - Comprobar con `./competicion --comprobar` y compilar en Release.
  - Escribir en `EXPERIMENTOS.md` la mediana con 1, 2, 4 y 8 hilos (y cuántos núcleos tiene el ordenador), con una explicación de esos tiempos.
  - Indicar en este README con cuántos hilos se mide. INDICACIÓN: La competición se ha medido con 1, 2, 4 y 8 hilos.
- Como es la parte más ligera, también ayuda a revisar `DISENO.md` y el código de los demás.
- **Hito 1:** buffer y empaquetadores creados y terminando bien con el cierre.

## Calendario

| Semana | Qué tiene que estar | Etiqueta |
|---|---|---|
| 1 | Proyecto base compilando, `Semaforo` con su prueba, las 3 clases con todos los hilos creados y terminando bien, modo `carrera` y predicción del experimento escrita | `hito1` |
| 2 | Sincronización completa, invariantes, resto de modos de fallo, resultado del experimento, competición, `DISENO.md` e `IA.md` | `final` |

## Normas de git

- Cada miembro hace **sus propios commits** a lo largo de las dos semanas.
- Pocos commits enormes al final, o un solo autor, penaliza.

## Defensa

Demostración, cambio en vivo sin IA y preguntas individuales: **cualquier miembro debe poder explicar cualquier parte del código**.
