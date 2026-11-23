
## 2026-11-22 — Vicent David Burgal — Competición y uso de vectores con hilos

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
