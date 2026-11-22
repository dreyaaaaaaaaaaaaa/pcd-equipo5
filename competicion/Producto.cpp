#include "Producto.h"

#include "Valor.h"

#include <thread>
#include <vector>

// Versión secuencial de partida: un solo hilo, ignora "hilos".

// // Cambiadla por la vuestra.
// std::uint64_t producto(int hilos) {
//     (void)hilos;
//     std::uint64_t r = 1;
//     for (std::uint64_t i = 0; i < N_VALORES; ++i) {
//         r = (r * valor(i)) % MODULO;
//     }
//     return r;
// }

std::uint64_t producto(int hilos) {
    // Si solo hay un hilo, hacemos el cálculo normal
    if (hilos <= 1) {
        std::uint64_t resultado = 1;

        for (std::uint64_t i = 0; i < N_VALORES; ++i) {
            resultado = (resultado * valor(i)) % MODULO;
        }

        return resultado;
    }

// No creamos más hilos que valores.
int numeroHilos = hilos;

    if (numeroHilos > N_VALORES) {
        numeroHilos = N_VALORES;
    }

// Cada hilo guardará aquí su resultado parcial.
std::vector<std::uint64_t> resultadosParciales(numeroHilos);

// Creamos un vector con espacio para todos los hilos.
std::vector<std::thread> hilosCreados(numeroHilos);

// Calculamos cuántos valores le tocan a cada hilo.
std::uint64_t valoresPorHilo = N_VALORES / numeroHilos;

// El primer hilo empieza en el valor 0.
std::uint64_t comienzo = 0;

for (int numeroHilo = 0; numeroHilo < numeroHilos; ++numeroHilo) {
    // Calculamos hasta qué valor llega este hilo.
    std::uint64_t final = comienzo + valoresPorHilo;

    // El último hilo se queda con los valores que sobran.
    if (numeroHilo == numeroHilos - 1) {
        final = N_VALORES;
    }

    // Guardamos el principio y el final de este hilo.
    std::uint64_t principioHilo = comienzo;
    std::uint64_t finalHilo = final;

    
        // Creamos el hilo y lo guardamos en su posición.
    hilosCreados[numeroHilo] = std::thread(
        [&resultadosParciales, numeroHilo, principioHilo, finalHilo]() {
            std::uint64_t resultadoParcial = 1;

            for (std::uint64_t i = principioHilo; i < finalHilo; ++i) {
                resultadoParcial = (resultadoParcial * valor(i)) % MODULO;
            }

            resultadosParciales[numeroHilo] = resultadoParcial;
        }
    );
    // El siguiente hilo empieza donde termina este.
    comienzo = final;
    
}
    
    // Esperamos a que terminen todos los hilos.
    for (int numeroHilo = 0; numeroHilo < numeroHilos; ++numeroHilo) {
        hilosCreados[numeroHilo].join();
    }

    // Multiplicamos los resultados de todos los hilos.
    std::uint64_t resultadoFinal = 1;

    for (int numeroHilo = 0; numeroHilo < numeroHilos; ++numeroHilo) {
        resultadoFinal = (resultadoFinal * resultadosParciales[numeroHilo]) % MODULO;
    }

    return resultadoFinal;
}

