#pragma once
#include "Contexto.h"
#include <array>
#include <condition_variable>
#include <mutex>

class Pasillo {
public:
    static constexpr int CELDAS = 12;   // celdas del pasillo (0..11)
    static constexpr int LIBRE = -1;    // valor de una celda sin robot
    static constexpr int MAX_DENTRO = CELDAS - 1;   // como mucho 11 robots dentro, asi siempre queda una celda libre

Pasillo(const Contexto& ctx, int cargadores);

        //CELDAS
    void entrar(int robot);                 // el robot entra al pasillo por la celda 0
    void ocupar(int robot, int celda);      // el robot ocupa una celda
    bool intentarOcupar(int robot, int celda);  // como ocupar pero sin esperar: false si esta ocupada (modo livelock)
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
    std::array<std::condition_variable, CELDAS> _cvCelda;   // en _cvCelda[c] espera el robot que quiere la celda c
    std::condition_variable _cvEntrada;     // aqui esperan los robots que quieren entrar
    int _dentro = 0;                        // cuantos robots hay dentro del pasillo
    long _turnoSiguiente = 0;               // turno que le toca al siguiente robot que llega a la entrada
    long _turnoActual = 0;                  // turno que puede entrar ahora
    bool _sinLimite;                        // modo interbloqueo: no se limita cuantos entran
    void marcarCelda(int robot, int celda); // apunta la celda y comprueba el invariante (con _m cogido)

    //CARGADORES
    std::mutex _mCargadores;     // Protege los datos compartidos de los cargadores.
    std::condition_variable _cvCargadores;       // Permite que los robots esperen hasta que haya un cargador disponible.
    int _cargadoresLibres;       // Número de cargadores disponibles
    long _ticketSiguiente = 0;       // Ticket siguiente a asignar
    long _ticketAtendido = 0;       // Ticket que se está atendiendo

};