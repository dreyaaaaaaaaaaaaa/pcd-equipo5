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
    // Si solo hay un hilo, hacemos el cálculo normal.
    if (hilos <= 1) {
        std::uint64_t resultado = 1;

        for (std::uint64_t i = 0; i < N_VALORES; ++i) {
            resultado = (resultado * valor(i)) % MODULO;
        }

        return resultado;
    }