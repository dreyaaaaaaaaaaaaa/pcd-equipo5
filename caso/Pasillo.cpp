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
    const long miTurno = _turnoSiguiente++; // cojo numero, se entra por orden de llegada
    _cvEntrada.wait(lk, [this, miTurno] {
        return miTurno == _turnoActual            // es mi turno
            && _dentro < MAX_DENTRO               // hay sitio (como mucho 11 dentro)
            && _celdas[0] == LIBRE                // la entrada esta libre
            && _celdas[CELDAS - 1] == LIBRE;      // y no viene nadie por la 11 (el de dentro tiene prioridad)
    });
    marcarCelda(robot, 0);
    _dentro++;
    _turnoActual++;                         // paso el turno al siguiente
    _cvEntrada.notify_all();                // para que el del siguiente turno lo compruebe
}

void Pasillo::ocupar(int robot, int celda) {
    std::unique_lock<std::mutex> lk(_m);    // unique_lock porque wait tiene que poder soltarlo
    _cvCelda[celda].wait(lk, [this, celda] {    // espero mientras la celda este ocupada
        return _celdas[celda] == LIBRE;
    });
    marcarCelda(robot, celda);
}

void Pasillo::soltar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[celda] == robot) {          // solo la libero si de verdad es mia
        _celdas[celda] = LIBRE;
    }
    _cvCelda[celda].notify_one();           // aviso al robot de detras, que es el unico que puede querer esta celda
        if (celda == 0 || celda == CELDAS - 1) {
        _cvEntrada.notify_all();            // la 0 o la 11 han quedado libres, a lo mejor alguien puede entrar
    }
}

void Pasillo::salir(int robot) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[0] == robot) {
        _celdas[0] = LIBRE;
    }
    _dentro--;                              // un robot menos dentro
    _cvCelda[0].notify_one();               // aviso al robot de la celda 11, que quiere la 0
    _cvEntrada.notify_all();                // hay sitio y la celda 0 esta libre
}