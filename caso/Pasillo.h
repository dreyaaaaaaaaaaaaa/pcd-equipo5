#pragma once
#include "Contexto.h"
#include <array>
#include <condition_variable>
#include <mutex>

class Pasillo {
public:
    static constexpr int CELDAS = 12;   // celdas del pasillo (0..11)
    static constexpr int LIBRE = -1;    // valor de una celda sin robot

Pasillo(const Contexto& ctx, int cargadores);

        //CELDAS
    void entrar(int robot);                 // el robot entra al pasillo por la celda 0
    void ocupar(int robot, int celda);      // el robot ocupa una celda
    void soltar(int robot, int celda);      // el robot deja una celda que tenia
    void salir(int robot);                  // el robot deja la celda 0 y se va del pasillo

        //CARGADORES
    void ocuparCargador(int robot);     // Espera hasta que el robot pueda ocupar un cargador.
    void soltarCargador(int robot);     // Libera el cargador que estaba usando el robot.

private:

    const Contexto&_ctx;       //Guardas estadisitcas

    //CELDAS
    std::mutex _m;                          // candado de las celdas
    std::array<int, CELDAS> _celdas;        // que robot hay en cada celda (o LIBRE)

    //CARGADORES
    std::mutex _mCargadores;     // Protege los datos compartidos de los cargadores.
    std::condition_variable _cvCargadores;       // Permite que los robots esperen hasta que haya un cargador disponible.
    int _cargadoresLibres;       // Número de cargadores disponibles
    long _ticketSiguiente = 0;       // Ticket siguiente a asignar
    long _ticketAtendido = 0;       // Ticket que se está atendiendo

};