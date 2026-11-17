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
