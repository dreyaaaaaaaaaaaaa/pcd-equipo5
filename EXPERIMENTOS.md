Tras ejecutar build\competicion\Release\competicion.exe --comprobar sale:

medición 1: 3738.52 ms
resultado: 521537800
hilos: 1   mediana: 3738.52 ms
correcto:  521537800   OK

Tras build\competicion\Release\competicion.exe --hilos 1:

medición 1: 3859.71 ms
resultado: 521537800
hilos: 1   mediana: 3859.71 ms

Tras build\competicion\Release\competicion.exe --hilos 2:

medición 1: 2833.24 ms
resultado: 521537800
hilos: 2   mediana: 2833.24 ms

Tras build\competicion\Release\competicion.exe --hilos 4:

medición 1: 1651.78 ms
resultado: 521537800
hilos: 4   mediana: 1651.78 ms

Tras build\competicion\Release\competicion.exe --hilos 8:

medición 1: 936.158 ms
resultado: 521537800
hilos: 8   mediana: 936.158 ms

## Experimento de la competición

Se ha medido el tiempo de ejecución de producto(int hilos) con 1, 2, 4 y 8 hilos.

Para cada número de hilos se ha ejecutado el programa una vez y se ha tomado la mediana que muestra el propio programa.

| Hilos | Mediana (ms) |
|---:|---:|
| 1 | 3940.41 |
| 2 | 2835.68 |
| 4 | 1698.16 |
| 8 | 882.687 |

El resultado obtenido ha sido 521537800 y la comprobación con --comprobar ha indicado OK.

Al aumentar el número de hilos, el trabajo de calcular el producto de los 2.000.000 valores se reparte entre varios hilos, por eso el tiempo disminuye al usar más hilos dado que los cálculos se realizan de forma concurrente.

El ordenador utilizado tiene 10 núcleos físicos y 16 núcleos lógicos.

## salidas de --estres 10 del caso almacen

estas salidas son reales. las saque en mi mac de 10 nucleos, con la version de main de ahora compilada en Release. no las he tocado.

### sin fallo

orden: `./build/caso/almacen --estres 10`

```
almacen (versión correcta)

--estres 10: 10 OK, 0 con invariantes rotos (misma configuración en todas).
```

salen bien las 10 y no se rompe ningun invariante.

### --fallo carrera

orden con la configuracion por defecto: `./build/caso/almacen --fallo carrera --estres 10`

```
almacen --fallo carrera
  ejecución 5 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 7 de 10: INVARIANTE ROTO -> existencias < 0

--estres 10: 8 OK, 2 con invariantes rotos (misma configuración en todas).
```

con la configuracion por defecto se rompe en 2 de 10. en otro ordenador del equipo esta misma orden dio 10 OK. para que falle casi siempre hay que bajar las existencias y subir los generadores: `./build/caso/almacen --fallo carrera --existencias 1 --generadores 4 --estres 10`

```
almacen --fallo carrera
  ejecución 1 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 2 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 3 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 4 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 5 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 6 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 7 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 9 de 10: INVARIANTE ROTO -> existencias < 0
  ejecución 10 de 10: INVARIANTE ROTO -> existencias < 0

--estres 10: 1 OK, 9 con invariantes rotos (misma configuración en todas).
```

con esa configuracion se rompe en 9 de 10.

### --fallo interbloqueo

orden: `./build/caso/almacen --fallo interbloqueo --estres 10`

```
almacen --fallo interbloqueo

*** VIGILANTE (ejecución 1 de 10): sin progreso durante 2000 ms. Posible interbloqueo, livelock o despertar perdido. ***

=== Esperas por tipo de hilo ===
  robot (celda siguiente)                      n=   414  media=    9.62 ms  max=   12.74 ms
  robot (pedido)                               n=    41  media=    0.00 ms  max=    0.00 ms
=== Métricas ===
  entregas                                     29
  movimientos                                  414
  paquetes                                     29
  pedidos creados                              48
=== Invariantes ===
  OK
Se termina el proceso: los hilos bloqueados no se pueden despertar.
```

el vigilante corta el programa en la primera ejecucion y sale con codigo 3, asi que no llegan a hacerse las 10. es lo que se espera. los robots se quedan esperando una celda que no se libera nunca y las entregas se paran en 29.

### --fallo livelock

orden: `./build/caso/almacen --fallo livelock --estres 10`

```
almacen --fallo livelock

*** VIGILANTE (ejecución 1 de 10): sin progreso durante 2000 ms. Posible interbloqueo, livelock o despertar perdido. ***

=== Esperas por tipo de hilo ===
  robot (pedido)                               n=    12  media=    0.00 ms  max=    0.00 ms
=== Métricas ===
  movimientos                                  4067
  pedidos creados                              20
=== Invariantes ===
  OK
Se termina el proceso: los hilos bloqueados no se pueden despertar.
```

tambien lo corta el vigilante en la primera ejecucion, con codigo 3. la diferencia es que aqui los robots no estan parados: hacen 4067 movimientos y no entregan ni un pedido.

### --fallo inanicion

orden: `./build/caso/almacen --fallo inanicion --cargadores 1 --pedidos 100 --existencias 30 --estres 10`

```
almacen --fallo inanicion

--estres 10: 10 OK, 0 con invariantes rotos (misma configuración en todas).
```

el modo inanicion todavia no esta hecho en el codigo, asi que se comporta igual que la version correcta y no se ve ninguna espera larga en el cargador. hay que repetir esta salida cuando alejandro lo termine.

### por que la misma configuracion falla unas veces si y otras no

en el modo carrera el generador mira si hay existencias, suelta el candado y luego resta sin volver a mirar. solo falla si otro generador reserva justo en ese hueco. eso depende de como el sistema operativo vaya alternando los hilos, que no lo controlamos y cambia en cada ejecucion. por eso la misma orden sale bien unas veces y mal otras, y por eso en otro ordenador salio distinto.

con menos existencias y mas generadores peleando por ellas, es mas facil que dos coincidan en ese hueco. con 1 existencia y 4 generadores casi siempre falla, y con la configuracion por defecto solo de vez en cuando.

que una ejecucion salga bien no prueba que el programa este bien. por eso se repite con --estres.




## Modo carrera

    almacen --fallo carrera --existencias 1 --generadores 4 --estres 10

    ejecución 1 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 2 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 3 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 5 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 6 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 7 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 8 de 10: INVARIANTE ROTO -> existencias < 0
    ejecución 9 de 10: INVARIANTE ROTO -> existencias < 0

    --estres 10: 2 OK, 8 con invariantes rotos (misma configuración en todas).

Con la configuración por defecto (2 generadores y 3 existencias) casi nunca falla,
porque con la fórmula (7g + k) mod 6 los generadores piden productos distintos en
el mismo paso y el gestor repone cada 5 ms. Con 1 existencia y 4 generadores es
mucho más fácil que dos vayan a la vez a por la última unidad.

Falla unas veces sí y otras no porque depende del orden en que el sistema operativo
ejecuta los hilos: solo falla si un generador entra justo entre la consulta y la
resta del otro.


## Experimento del almacén: predicción previa

Se compararán las entregas por segundo con `--robots 4`, `--robots 8` y `--robots 12`, manteniendo iguales las demás opciones.

Voy a probar el programa con 4, 8 y 12 robots. Creo que con 8 robots habrá más entregas que con 4, porque trabajan más robots a la vez. Con 12 espero que mejore poco o que se quede parecido a 8, porque en el pasillo solo pueden estar 11 robots a la vez. Además, todos comparten los cargadores y el sitio donde dejan los productos. Esto es lo que creo que pasará; todavía no son resultados reales.


## Experimento del almacén: resultado

Orden (igual en los tres casos, solo cambia --robots):

Almacen --robots N --pedidos 200 --existencias 30

| Robots | Medidas (s) | Mediana (s) | Entregas | Entregas por segundo |
|---:|---|---:|---:|---:|
| 4  | 22,51 · 22,35 · 22,56 | 22,51 | 400 | 17,8 |
| 8  | 17,19 · 16,99 · 17,24 | 17,19 | 400 | 23,3 |
| 12 | 17,37 · 17,04 · 17,21 | 17,21 | 400 | 23,2 |

La predicción se cumple. De 4 a 8 robots las entregas por segundo suben un 30 %, porque
trabajan más robots a la vez. De 8 a 12 no mejoran: en el pasillo solo puede haber 11 robots
a la vez y todos comparten los cargadores y el buffer de empaquetado, así que los robots
de más se quedan esperando.

Los tiempos son mayores de lo que darían 12 movimientos de 1 ms porque en Windows
sleep_for(1 ms) dura bastante más que 1 ms. Esto afecta igual a los tres casos, así que
la comparación sigue siendo válida.