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

    
 // Creamos los cuatro hilos. NO SIRVE, AFUERA
    std::thread hilo0([&resultadoHilo0, inicio0, fin0]() {
        for (std::uint64_t i = inicio0; i < fin0; ++i) {
            resultadoHilo0 = (resultadoHilo0 * valor(i)) % MODULO;
        }
    });

    std::thread hilo1([&resultadoHilo1, inicio1, fin1]() {
        for (std::uint64_t i = inicio1; i < fin1; ++i) {
            resultadoHilo1 = (resultadoHilo1 * valor(i)) % MODULO;
        }
    });

    std::thread hilo2([&resultadoHilo2, inicio2, fin2]() {
        for (std::uint64_t i = inicio2; i < fin2; ++i) {
            resultadoHilo2 = (resultadoHilo2 * valor(i)) % MODULO;
        }
    });

    std::thread hilo3([&resultadoHilo3, inicio3, fin3]() {
        for (std::uint64_t i = inicio3; i < fin3; ++i) {
            resultadoHilo3 = (resultadoHilo3 * valor(i)) % MODULO;
        }
    });

    // Esperamos a que terminen los cuatro hilos.
    hilo0.join();
    hilo1.join();
    hilo2.join();
    hilo3.join();
    
// Queda: EL RESULTADO FINAL

