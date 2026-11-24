
## 2026-11-17 - 2026-11-23 — Vicent David Burgal — Competición y uso de vectores con hilos

**Herramienta:** Perplexity.

**Para qué la he usado:**
- Entender qué es una variable `uint64_t` y por qué no se puede sustituir por `float` o `double` en la competición.
- Comprobar si mi primera versión de `Producto.cpp` cumplía las condiciones del enunciado.
- Ver qué faltaba para que la función usara “como mucho `hilos` hilos” y no creara siempre cuatro hilos.
- Aprender a usar vectores y cómo combinarlos con hilos para guardar el resultado parcial de cada hilo.
- Pedir ayuda para terminar la función `producto(int hilos)`, especialmente porque no sabía cómo funcionaban las lambdas.
- Comprobar si era razonable usar bucles para que cada hilo calculara una parte de los valores y para combinar después los resultados parciales.
- Entender qué significan las variables entre corchetes de una lambda, como `&resultadosParciales`, `numeroHilo`, `principioHilo` y `finalHilo`.
- Resolver errores al ejecutar `competicion.exe`: entender que el ejecutable estaba en `build\competicion\Release\` y que había que ejecutarlo desde la terminal con `--comprobar`.
- Hacer la build con CMake para poder probarlo usando CMD

**Qué se incorporó:** 
La versión paralelizada de `producto(int hilos)` en `competicion/Producto.cpp`, dividendo los 2.000.000 valores en bloques, calculando un producto parcial por hilo y combinando los resultados módulo `MODULO`. También la comprobación con `.\build\competicion\Release\competicion.exe --comprobar` y la preparación de las mediciones con 1, 2, 4 y 8 hilos.

**Qué se descartó:** Una versión que usaba un número concreto de hilos, porque no se adaptaba bien a otros valores. También se descartó usar `float` o `double`, porque pueden perder precisión en el cálculo.

**Ficheros:** competicion/Producto.cpp, EXPERIMENTOS.md, README.md, IA.md.

---

---

## 2026-11-22 — Ange Lucas Benoit — Organización del trabajo y versión del hito 1

**Herramienta:** Claude Code (Claude Opus 5.5), app de escritorio y extensión de VS Code.

**Para qué la he usado:**
- Organizar cómo empezar mi parte: qué hacer primero (diseño y tabla de predicados), qué va al hito 1 y qué a la semana 2, y en qué commits dividirlo.
- Ver qué temas de la asignatura entran en mi parte:
  - Tema 3 (hilos): cada robot es un hilo y `ejecutar()` es su cuerpo.
  - Tema 4 (carreras y exclusión mutua con RAII): el array de celdas protegido con `_m` y `lock_guard`.
  - Tema 6 (monitores): `Pasillo` como monitor, con una variable de condición por celda (semana 2).
  - Tema 7 (interbloqueo): las 4 condiciones de Coffman en el pasillo circular, y cómo el límite de 11 robots rompe la espera circular.
  - Tema 8 (inanición y livelock): turnos FIFO en la entrada, prioridad del robot de la celda 11 y el modo `livelock`.
- Coordinar con Alejandro la interfaz de `Pasillo`, que compartimos, y revisar los errores de compilación de los ficheros.
- El código del hito 1, que he escrito yo commit a commit a partir de lo que me dio la IA: constructor y celdas de `Pasillo` (entrar, ocupar, soltar y salir, todavía sin esperas), `Robot` con el ciclo completo, el cambio del `emplace_back` en `main` y el renombrado de `pasillo.*` a `Pasillo.*` (este último con Claude Code en VS Code).

**Qué se incorporó:** En el hito 1 las celdas todavía no esperan, así que con `--estres 20` salen violaciones de celda. Es lo esperado hasta que esté la sincronización de la semana 2.

**Qué se descartó:** nada de momento.

**Ficheros:** caso/Pasillo.h, caso/Pasillo.cpp, caso/Robot.h, caso/Robot.cpp, caso/main.cpp, DISENO.md, AI.md.

---

## 2026-11-18 a 2026-11-23 — Álvaro Rovira de las Heras — Semaforo, prueba_semaforo y main del caso

**Herramienta:** Claude Code (Claude Sonnet 5.5).

**Para qué la he usado:**
- Entender cerrojos, variables de condición y semáforos, y cómo está montado el proyecto.
- El código de `Semaforo.cpp` (`acquire`, `release` y `tryAcquireFor`) lo he hecho con ayuda de la IA, que me explicó cada línea. Los comentarios son míos y me corrigió dos que estaban mal.
- Comprobar que compila y funciona, y redactar con salidas reales la sección de `--estres 10` de `EXPERIMENTOS.md` y la de compilar y ejecutar del `README.md`.
- Hacer mis commits y push desde mi usuario de git, siempre después de pedírselo yo.

**Qué se incorporó:** las tres funciones de `Semaforo.cpp`, `prueba_semaforo/main.cpp`, `caso/main.cpp`, el nombre `almacen` en `caso/CMakeLists.txt` y las secciones de `EXPERIMENTOS.md` y `README.md` que he añadido.

**Qué se descartó:** mi primer `tryAcquireFor`, que no esperaba el plazo.

**Ficheros:** semaforo/Semaforo.cpp, prueba_semaforo/main.cpp, caso/main.cpp, caso/CMakeLists.txt, EXPERIMENTOS.md, README.md, AI.md.



---

## 2026-11-23 — Víctor Díez Sánchez — CentroPedidos: inventario, cola de pedidos, generador, gestor y modo carrera

**Herramienta:** Claude (claude.ai).

**Para qué la he usado:**
- Organizar cómo empezar mi parte: qué me tocaba según el enunciado y el reparto, qué iba al hito 1 y en qué commits dividirlo.
- Ver qué temas de la asignatura entran en mi parte:
  - Tema 3 (hilos): el generador y el gestor son hilos, y `bucleGenerador` y `bucleGestor` son su cuerpo.
  - Tema 4 (carreras y exclusión mutua con RAII): el inventario protegido con `shared_mutex`, comprobando y reservando en la misma sección crítica, y el modo `carrera`, que lo hace en dos secciones distintas.
  - Tema 5 (semáforos): la cola de pedidos, con los huecos en nuestro `Semaforo` y los pedidos en `std::counting_semaphore`, siempre el semáforo antes que el mutex.
  - Tema 6 (monitores): el gestor, que espera con `wait_for` y se despierta al momento con `notify_one` en vez de usar `sleep`.
- Entender por qué con la configuración por defecto el modo `carrera` casi no falla: con (7g + k) mod 6, los generadores piden productos distintos en el mismo paso.
- Revisar mis ficheros antes de subirlos y recuperar los métodos que perdí en un conflicto al guardar.
- El código de mi parte, que he escrito yo commit a commit a partir de lo que me dio la IA, ajustándolo para usar solo lo que aparece en el código base del profesor.

**Qué se incorporó:** `CentroPedidos` con el inventario, la cola de pedidos, el generador, el gestor y el modo `carrera`, que se ve con `--existencias 1 --generadores 4` (2 OK y 8 con invariantes rotos en `--estres 10`). También mi tabla de `DISENO.md` y mi apartado de `EXPERIMENTOS.md`.

**Qué se descartó:** `std::deque`, constantes `static const` y trazas, porque no aparecen en el código base. Tampoco se cambió el código del modo carrera: se documenta con qué configuración se ve.

**Ficheros:** caso/CentroPedidos.h, caso/CentroPedidos.cpp, DISENO.md, EXPERIMENTOS.md, AI.md.



---

## 2026-11-21 a 2026-11-23 — Alejandro Castro Espín — Pasillo: monitor de cargadores

**Herramienta:** ChatGPT / Codex.

**Para qué la he usado:**

- Ver qué temas de la asignatura entran en mi parte:- Tema 3 (hilos): cada robot ejecuta su trabajo en un hilo y solicita cargar cuando su batería queda por debajo de 12 movimientos.
- Tema 4 (carreras y atomicidad): el número de plazas libres y los turnos deben actualizarse de forma protegida para que dos robots no reserven la misma plaza.
- Tema 5 (exclusión mutua con RAII): el mutex del monitor protege el estado de los cargadores. Debo asegurar que los cerrojos se liberen mediante RAII, sin llamar a `unlock()` manualmente.
- Tema 7 (monitores y variables de condición): Pasillo gestiona los cargadores con un mutex, una variable de condición y tickets para atender a los robots por orden de llegada.
- Tema 8 (inanición): el modo `--fallo` inanicion entrega la plaza libre al robot de menor número que esté esperando, para mostrar cómo aumenta su tiempo máximo de espera.
- Entender cómo hacer que los cargadores se asignen por tickets en orden de llegada y cómo activar el modo inanicion para dar prioridad al robot de menor número que esté esperando.

- Corregir errores de compilación en `Pasillo.h` y `Pasillo.cpp`, como un punto y coma que faltaba, un nombre de variable mal escrito y el cierre de la espera con condición.

- Entender cómo registrar la espera del cargador en las estadísticas y cómo liberar el mutex usando el ámbito de `unique_lock`, sin llamar a `unlock()` manualmente.

- Averiguar por qué no se podía ejecutar `almacen.exe`: Windows mostró que una directiva de Control de aplicaciones bloqueaba el programa.

**Qué se incorporó:**  dos funciones de `Pasillo.cpp`, Los nombres de `Pasillo` y `Robot`  en `caso/CMakeLists.txt` y las secciones de `EXPERIMENTOS.md` y `README.md` que he añadido.


**Qué se descartó:** la llamada manual a `lock.unlock()`, porque el mutex se libera automáticamente al salir de la sección protegida. 

**Ficheros:** caso/Pasillo.h, caso/Pasillo.cpp, DISENO.md. EXPERIMENTOS.md, AI.md, caso/Robot.h, caso/Robot.cpp