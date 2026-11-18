#include "Semaforo.h"

Semaforo::Semaforo(int valorInicial) : _valor(valorInicial) {}

void Semaforo::acquire() {
    std::unique_lock<std::mutex> lock(_m);
    while (_valor <= 0) {
        _cv.wait(lock);
    }
    --_valor;
}

void Semaforo::release() {
    std::unique_lock<std::mutex> lock(_m);
    ++_valor;
    _cv.notify_one();
}

bool Semaforo::tryAcquireFor(std::chrono::milliseconds) {
    return false;
}
