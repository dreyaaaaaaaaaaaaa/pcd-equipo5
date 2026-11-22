Vicent: Qué es una variable "uint64_t"?
Vicent: Este código cumple con las condiciones?
Vicent: Cómo puedo hacer que las cumpla?
Vicent: Por favor, enséñame a usar vectores, y si me enseñas a usarlos junto hilos ya es un favor enorme
Vicent: Por favor, necesito que me ayudes a terminar la función, no tengo ni la más mínima idea de usar lambdas y no puedo más
Vicent: Hacer que los hilos vayan con bucles no es descabellado, no? Y con un bucle también podemos calcular el resultado final, cierto?

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
