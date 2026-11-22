#include "Semaforo.h"

Semaforo::Semaforo(int valorInicial) : _valor(valorInicial) {}

void Semaforo::acquire() {
    std::unique_lock<std::mutex> lock(_m); //lock coge el cerrojo _m
    while (_valor <= 0) { //mientras no hayan permisos 
        _cv.wait(lock); //el hilo se duerme soltando el cerrojo, al despertar lo recupera
    }
    --_valor; //se resta 1 al valor
}

void Semaforo::release() {
    std::unique_lock<std::mutex> lock(_m); //mismo cerrojo que antes 
    ++_valor; //se suma 1 al valor
    _cv.notify_one(); //se avisa al hilo que hay un permiso para que despierte 
}

bool Semaforo::tryAcquireFor(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(_m);
    auto limite = std::chrono::steady_clock::now() + timeout;
    while (_valor <= 0) {
        if (_cv.wait_until(lock, limite) == std::cv_status::timeout) {
            if (_valor <= 0) {
                return false;
            }
        }
    }
    --_valor;
    return true;
}