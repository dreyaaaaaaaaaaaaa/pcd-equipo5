# Diseño — Caso 5: Almacén robotizado

## Tabla de predicados

Cada fila es una operación de un monitor que puede bloquear al hilo que la llama.
«Espera mientras…» es la condición del `wait(lk, predicado)` escrita en negativo
(el hilo sigue cuando deja de cumplirse). «Quién avisa» es la operación que hace
falso ese predicado y llama a `notify_*`.

### Pasillo — celdas (Ange Lucas Benoit)

Estado protegido por `Pasillo::_m` (un único mutex para todas las celdas):

| Miembro | Significado |
|---|---|
| `_celdas[12]` | id del robot que ocupa cada celda, o `LIBRE` (-1) |
| `_dentro` | robots que están dentro del pasillo |
| `_turnoSiguiente` / `_turnoActual` | turnos de la entrada (orden de llegada, FIFO) |
| `_cvCelda[12]` | en `_cvCelda[c]` espera el robot que quiere ocupar la celda `c` |
| `_cvEntrada` | esperan los robots que quieren entrar por la celda 0 |
| `_sinLimite` | `true` con `--fallo interbloqueo` o `--fallo livelock`: no hay límite de robots ni prioridad |
| `marcarCelda(r, c)` | método privado: apunta la celda y llama a `violacion` si ya estaba ocupada (siempre con `_m` cogido) |

`MAX_DENTRO = CELDAS - 1 = 11`.

**Versión correcta**

| Operación | Espera mientras… | Quién avisa |
|---|---|---|
| `entrar(r)` | `miTurno != _turnoActual` **o** `_dentro == MAX_DENTRO` **o** `_celdas[0] != LIBRE` **o** `_celdas[11] != LIBRE` | `soltar(·, 0)`, `soltar(·, 11)` y `salir(·)` → `_cvEntrada.notify_all()`; el propio `entrar` al pasar el turno → `_cvEntrada.notify_all()` |
| `ocupar(r, c)` | `_celdas[c] != LIBRE` | el robot que suelta `c`: `soltar(·, c)`, o `salir(·)` si `c == 0` → `_cvCelda[c].notify_one()` |
| `soltar(r, c)` | — (no espera) | — |
| `salir(r)` | — (no espera) | — |

Notas:

- `_cvCelda[c]` usa `notify_one` porque solo puede haber **un** hilo esperando en ella:
  como los robots solo avanzan, la celda `c` solo la pide el robot que está en `c-1`
  (los que quieren entrar esperan en `_cvEntrada`, no en `_cvCelda[0]`).
- `_cvEntrada` usa `notify_all` porque esperan varios robots con turnos distintos y solo
  puede pasar el del turno actual; con `notify_one` podría despertarse otro, volver a
  dormirse y perderse el aviso.
- Los tiempos (1 ms por movimiento) los duerme el `Robot` **fuera** del monitor. Ninguna
  operación de `Pasillo` duerme con el cerrojo cogido.
- La espera de la entrega (buffer lleno) la hace el robot en `CentroPedidos`, ocupando la
  celda 0 pero **sin** tener cogido `Pasillo::_m`. Nunca se llama a otro monitor con el
  cerrojo del pasillo cogido.

**Modos de fallo**

| Modo | Operación | Espera mientras… | Efecto |
|---|---|---|---|
| `interbloqueo` | `entrar(r)` | `_celdas[0] != LIBRE` (sin límite de robots dentro ni prioridad) | pueden entrar los 12 robots |
| `interbloqueo` | `ocupar(r, c)` | `_celdas[c] != LIBRE`, sin plazo | con 12 robots dentro, espera circular: el vigilante termina con código 3 |
| `livelock` | `entrar(r)` | igual que en `interbloqueo` | pueden entrar los 12 robots |
| `livelock` | `intentarOcupar(r, c)` | no espera: devuelve `false` si está ocupada | el robot retrocede si puede, duerme 2 ms y reintenta; nadie se bloquea pero nadie entrega |

### Por qué la versión correcta no tiene interbloqueo, inanición ni livelock

- **Interbloqueo.** Como mucho hay 11 robots dentro y cada uno que espera tiene una sola
  celda, así que siempre hay al menos una celda libre. El primer robot por detrás de esa
  celda libre puede avanzar (o, si está en la celda 0 entregando, espera al buffer, que los
  empaquetadores vacían sin depender del pasillo). Se rompe la condición de **espera
  circular**.
- **Inanición dentro del pasillo.** La celda `c+1` (con `c+1 != 0`) solo la pide el robot
  de la celda `c`: no tiene competencia. La celda 0 la piden el robot de la 11 y los que
  quieren entrar, pero quien quiere entrar espera mientras la celda 11 esté ocupada
  (prioridad del que ya está dentro, como en una rotonda).
- **Inanición en la entrada.** Los robots entran por turno de llegada (`_turnoSiguiente` /
  `_turnoActual`).
- **Livelock.** En la versión correcta nadie retrocede ni reintenta: el que no puede
  avanzar se bloquea en su variable de condición hasta que le avisan.

### Cómo se mueve el robot (Robot::ejecutar)

1. `sacarPedido()`; si devuelve -1, el robot termina.
2. `entrar()`: espera su turno y ocupa la celda 0.
3. Vuelta de 12 movimientos. En cada uno: `ocupar(siguiente)`, 1 ms de movimiento y `soltar(actual)`, es decir, la regla 1 tal cual.
   Se lleva la cuenta con `recorrido` (0..12) porque en el modo livelock el robot puede retroceder.
4. `dejarProducto()` **antes** de `salir()`: si el buffer está lleno, el robot espera ocupando la celda 0, como pide el enunciado.
   Esa espera la hace `CentroPedidos`, sin tener cogido `_m`.
5. Si la batería es < 12: `ocuparCargador()`, 5 ms y `soltarCargador()`. El robot ya está fuera del pasillo,
   así que nunca tiene a la vez una celda y un cargador.

La batería es un miembro privado de `Robot` y solo la toca su propio hilo, así que no necesita mutex.

### Pruebas

| Orden | Resultado esperado |
|---|---|
| `almacen --estres 20` | 20 OK, 0 con invariantes rotos |
| `almacen --fallo interbloqueo` | el vigilante termina con código 3: con 12 robots dentro, todos esperan al de delante |
| `almacen --fallo livelock` | el vigilante termina con código 3: `movimientos` sigue subiendo pero `entregas` no |

### Pasillo — cargadores (Alejandro Castro Espín)

_Pendiente._

### CentroPedidos (Víctor Díez Sánchez y Vicent David Burgal)

_Pendiente._

## Interfaz pública (acordada)

Pedido = `int` con el número de producto (0..5); -1 es el pedido de fin (lo mete `meterFin()`).
Así no hace falta una cuarta clase.

```cpp
class Pasillo {
public:
    static constexpr int CELDAS = 12;
    static constexpr int LIBRE = -1;

    Pasillo(const Contexto& ctx, int cargadores);

    // Celdas (Ange)
    void entrar(int robot);                    // espera su turno y ocupa la celda 0
    void ocupar(int robot, int celda);         // espera a que la celda esté libre y la ocupa
    bool intentarOcupar(int robot, int celda); // sin esperar (solo modo livelock)
    void soltar(int robot, int celda);         // libera una celda que ocupa
    void salir(int robot);                     // libera la celda 0 y deja el pasillo

    // Cargadores (Alejandro)
    void ocuparCargador(int robot);            // espera plaza, por orden de llegada
    void soltarCargador(int robot);
};

class CentroPedidos {
public:
    int sacarPedido();                         // bloquea si la cola está vacía; -1 = terminar
    void dejarProducto(int producto);              // bloquea si el buffer de empaquetado está lleno
    // ... resto: generadores, gestor, empaquetadores
};

class Robot {
public:
    Robot(int id, const Contexto& ctx, Pasillo& pasillo, CentroPedidos& centro);
    void ejecutar();                           // cuerpo del hilo
};
```
