#include "Producto.h"

#include "Valor.h"

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

    // Resultado parcial de cada hilo, esto se modificará más tarde con los resultados de verdad de cada hilo
    std::uint64_t resultadoHilo1 = 1;
    std::uint64_t resultadoHilo2 = 1;
    std::uint64_t resultadoHilo3 = 1;
    std::uint64_t resultadoHilo4 = 1;

    // Dividimos los valores en cuatro partes

    std::uint64_t parte = N_VALORES / 4; // Esto nos calcula cuántos valores le tocan a cada hilo

    // Como vale 2000000, cada hilo se encargará de 500000 valores

    std::uint64_t inicio1 = 0;
    std::uint64_t fin1 = parte; // 500000

    std::uint64_t inicio2 = fin1;
    std::uint64_t fin2 = fin1 + parte; // 1000000

    std::uint64_t inicio3 = fin2;
    std::uint64_t fin3 = fin2 + parte; // 1500000

    std::uint64_t inicio4 = fin3;
    std::uint64_t fin4 = N_VALORES; // 2000000
    
// Queda: CALCULAR LOS 4 HILOS Y EL RESULTADO FINAL

