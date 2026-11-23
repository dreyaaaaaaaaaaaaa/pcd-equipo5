#include <chrono>
#include <mutex>
#include "Estadisticas.h"
#include "Pasillo.h"
#include "Config.h"
#include <thread>
#include <string>

//CONSTRUCTOR
Pasillo::Pasillo(const Contexto& ctx, int cargadores)
    : _ctx(ctx),
      _cargadoresLibres(cargadores) {   // empiezan todos los cargadores libres
    _celdas.fill(LIBRE);                // y todas las celdas vacias
}


//CELDAS
void Pasillo::marcarCelda(int robot, int celda) {
    if (_celdas[celda] != LIBRE) {          // si ya hay otro robot se rompe el invariante
        _ctx.estadisticas.violacion("celda " + std::to_string(celda) + ": entra el robot " +
                                    std::to_string(robot) + " y ya estaba el robot " +
                                    std::to_string(_celdas[celda]));
    }
    _celdas[celda] = robot;                 // apunto que la celda es mia
}

void Pasillo::entrar(int robot) {
    std::unique_lock<std::mutex> lk(_m);
    _cvEntrada.wait(lk, [this] {            // espero mientras el pasillo este lleno o la celda 0 ocupada
        return _dentro < MAX_DENTRO && _celdas[0] == LIBRE;
    });
    marcarCelda(robot, 0);
    _dentro++;
}

void Pasillo::ocupar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);     // candado de las celdas
    marcarCelda(robot, celda);
}

void Pasillo::soltar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[celda] == robot) {          // solo la libero si de verdad es mia
        _celdas[celda] = LIBRE;
    }
    if (celda == 0) {
        _cvEntrada.notify_all();            // la celda 0 ha quedado libre, a lo mejor alguien puede entrar
    }
}

void Pasillo::salir(int robot) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[0] == robot) {
        _celdas[0] = LIBRE;
    }
    _dentro--;                              // un robot menos dentro
    _cvEntrada.notify_all();                // hay sitio y la celda 0 esta libre
}