#include "Semaforo.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>

const int num_hilos = 12;
const int num_vueltas = 20;
const int valor_semaforo = 3;

int probar_semaforo_propio() {
    Semaforo semaforo(valor_semaforo);
    std::mutex cerrojo;
    int dentro = 0;
    int maximo = 0;
    std::vector<std::thread> hilos;
    for (int h = 0; h < num_hilos; h++) {
        hilos.push_back(std::thread([&]() {
            for (int v = 0; v < num_vueltas; v++) {
                semaforo.acquire();
                {
                    std::lock_guard<std::mutex> lk(cerrojo);
                    dentro++;
                    if (dentro > maximo) {
                        maximo = dentro;
                    }
                }
                std::this_thread::sleep_for(std::chrono::microseconds(300));
                {
                    std::lock_guard<std::mutex> lk(cerrojo);
                    dentro--;
                }
                semaforo.release();
            }
        }));
    }
    for (int h = 0; h < num_hilos; h++) {
        hilos[h].join();
    }
    return maximo;
}

int probar_semaforo_estandar() {
    std::counting_semaphore<valor_semaforo> semaforo(valor_semaforo);
    std::mutex cerrojo;
    int dentro = 0;
    int maximo = 0;
    std::vector<std::thread> hilos;
    for (int h = 0; h < num_hilos; h++) {
        hilos.push_back(std::thread([&]() {
            for (int v = 0; v < num_vueltas; v++) {
                semaforo.acquire();
                {
                    std::lock_guard<std::mutex> lk(cerrojo);
                    dentro++;
                    if (dentro > maximo) {
                        maximo = dentro;
                    }
                }
                std::this_thread::sleep_for(std::chrono::microseconds(300));
                {
                    std::lock_guard<std::mutex> lk(cerrojo);
                    dentro--;
                }
                semaforo.release();
            }
        }));
    }
    for (int h = 0; h < num_hilos; h++) {
        hilos[h].join();
    }
    return maximo;
}

int main()
{
    int fallos = 0;

    int maximo_propio = probar_semaforo_propio();
    std::cout << "Semaforo: maximo de hilos dentro = " << maximo_propio << "\n";
    if (maximo_propio > valor_semaforo) {
        std::cout << "FALLA: con Semaforo hubo mas de " << valor_semaforo << " hilos dentro\n";
        fallos++;
    }

    int maximo_estandar = probar_semaforo_estandar();
    std::cout << "std::counting_semaphore<3>: maximo de hilos dentro = " << maximo_estandar << "\n";
    if (maximo_estandar > valor_semaforo) {
        std::cout << "FALLA: con std::counting_semaphore hubo mas de " << valor_semaforo << " hilos dentro\n";
        fallos++;
    }

    Semaforo propio_a_cero(0);
    if (propio_a_cero.tryAcquireFor(std::chrono::milliseconds(50))) {
        std::cout << "FALLA: Semaforo con valor 0 devolvio true en tryAcquireFor(50 ms)\n";
        fallos++;
    }

    std::counting_semaphore<valor_semaforo> estandar_a_cero(0);
    if (estandar_a_cero.try_acquire_for(std::chrono::milliseconds(50))) {
        std::cout << "FALLA: std::counting_semaphore con valor 0 devolvio true en try_acquire_for(50 ms)\n";
        fallos++;
    }

    if (fallos == 0) {
        std::cout << "OK\n";
        return 0;
    }
    return 1;
}
