#include "Semaforo.h"

Semaforo::Semaforo(int valorInicial) : _valor(valorInicial) {}

void Semaforo::acquire() {
    std::unique_lock<std::mutex> lock(_cerrojo); //lock coge _cerrojo
    while (_valor <= 0) { //mientras no hayan permisos 
        _espera.wait(lock); //el hilo se duerme soltando el cerrojo, al despertar lo recupera
    }
    --_valor; //se resta 1 al valor
}

void Semaforo::release() {
    std::unique_lock<std::mutex> lock(_cerrojo); //mismo cerrojo que antes 
    ++_valor; //se suma 1 al valor
    _espera.notify_one(); //se avisa al hilo que hay un permiso para que despierte 
}

bool Semaforo::tryAcquireFor(std::chrono::milliseconds) {
    return false;
}
