Tras ejecutar .\build\competicion\Release\competicion.exe --comprobar sale:

medici├│n 1: 3880.9 ms
resultado: 521537800
hilos: 1   mediana: 3880.9 ms
correcto:  521537800   OK

Tras .\build\competicion\Release\competicion.exe --hilos 1:

medici├│n 1: 3940.41 ms
resultado: 521537800
hilos: 1   mediana: 3940.41 ms

Tras .\build\competicion\Release\competicion.exe --hilos 2:

medici├│n 1: 2835.68 ms
resultado: 521537800
hilos: 2   mediana: 2835.68 ms

Tras .\build\competicion\Release\competicion.exe --hilos 4:

medici├│n 1: 1698.16 ms
resultado: 521537800
hilos: 4   mediana: 1698.16 ms

Tras .\build\competicion\Release\competicion.exe --hilos 8:

medici├│n 1: 882.687 ms
resultado: 521537800
hilos: 8   mediana: 882.687 ms

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