#include "Semaforo.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>

const int num_hilos = 12; //cuantos hilos prueban a la vez
const int num_vueltas = 20; //cuantas veces repite cada hilo
const int valor_semaforo = 3; //permisos que tiene el semaforo

int probar_semaforo_propio() { //prueba nuestro Semaforo y devuelve el maximo de hilos que hubo dentro a la vez
    Semaforo semaforo(valor_semaforo); //semaforo con 3 permisos
    std::mutex cerrojo; //protege dentro y maximo, que los tocan los 12 hilos
    int dentro = 0; //hilos dentro ahora mismo
    int maximo = 0; //mayor numero de hilos que ha habido dentro a la vez
    std::vector<std::thread> hilos; //lista donde se guardan los hilos
    for (int h = 0; h < num_hilos; h++) { //se crea un hilo por vuelta
        hilos.push_back(std::thread([&]() { //el hilo ejecuta una lambda que ve las variables de fuera por referencia
            for (int v = 0; v < num_vueltas; v++) { //cada hilo repite 20 vueltas
                semaforo.acquire(); //coge un permiso, si no hay espera
                {
                    std::lock_guard<std::mutex> lk(cerrojo); //coge el cerrojo, se suelta solo al cerrar la llave
                    dentro++; //hay un hilo mas dentro
                    if (dentro > maximo) { //si hay mas dentro que nunca
                        maximo = dentro; //se guarda como nuevo maximo
                    }
                }
                std::this_thread::sleep_for(std::chrono::microseconds(300)); //el hilo espera 300 microsegundos, simula que trabaja dentro
                {
                    std::lock_guard<std::mutex> lk(cerrojo); //coge el cerrojo otra vez
                    dentro--; //hay un hilo menos dentro, va antes del release para no contar de mas
                }
                semaforo.release(); //devuelve el permiso
            }
        }));
    }
    for (int h = 0; h < num_hilos; h++) { //se recorren los hilos
        hilos[h].join(); //se espera a que cada hilo acabe
    }
    return maximo; //devuelve el maximo de hilos que hubo dentro
}

int probar_semaforo_estandar() { //igual que la anterior pero con el semaforo de la biblioteca estandar
    std::counting_semaphore<valor_semaforo> semaforo(valor_semaforo); //semaforo estandar con 3 permisos
    std::mutex cerrojo; //protege dentro y maximo, que los tocan los 12 hilos
    int dentro = 0; //hilos dentro ahora mismo
    int maximo = 0; //mayor numero de hilos que ha habido dentro a la vez
    std::vector<std::thread> hilos; //lista donde se guardan los hilos
    for (int h = 0; h < num_hilos; h++) { //se crea un hilo por vuelta
        hilos.push_back(std::thread([&]() { //el hilo ejecuta una lambda que ve las variables de fuera por referencia
            for (int v = 0; v < num_vueltas; v++) { //cada hilo repite 20 vueltas
                semaforo.acquire(); //coge un permiso, si no hay espera
                {
                    std::lock_guard<std::mutex> lk(cerrojo); //coge el cerrojo, se suelta solo al cerrar la llave
                    dentro++; //hay un hilo mas dentro
                    if (dentro > maximo) { //si hay mas dentro que nunca
                        maximo = dentro; //se guarda como nuevo maximo
                    }
                }
                std::this_thread::sleep_for(std::chrono::microseconds(300)); //el hilo espera 300 microsegundos, simula que trabaja dentro
                {
                    std::lock_guard<std::mutex> lk(cerrojo); //coge el cerrojo otra vez
                    dentro--; //hay un hilo menos dentro, va antes del release para no contar de mas
                }
                semaforo.release(); //devuelve el permiso
            }
        }));
    }
    for (int h = 0; h < num_hilos; h++) { //se recorren los hilos
        hilos[h].join(); //se espera a que cada hilo acabe
    }
    return maximo; //devuelve el maximo de hilos que hubo dentro
}

int main() //aqui empieza el programa
{
    int fallos = 0; //cuenta las comprobaciones que fallan

    int maximo_propio = probar_semaforo_propio(); //prueba nuestro Semaforo y guarda el maximo que salio
    std::cout << "Semaforo: maximo de hilos dentro = " << maximo_propio << "\n"; //imprime ese maximo
    if (maximo_propio > valor_semaforo) { //si hubo mas de 3 dentro, el semaforo falla
        std::cout << "FALLA: con Semaforo hubo mas de " << valor_semaforo << " hilos dentro\n"; //dice que comprobacion ha fallado
        fallos++; //se suma un fallo
    }

    int maximo_estandar = probar_semaforo_estandar(); //lo mismo con el semaforo estandar
    std::cout << "std::counting_semaphore<3>: maximo de hilos dentro = " << maximo_estandar << "\n"; //imprime ese maximo
    if (maximo_estandar > valor_semaforo) { //misma comprobacion que antes
        std::cout << "FALLA: con std::counting_semaphore hubo mas de " << valor_semaforo << " hilos dentro\n"; //dice que comprobacion ha fallado
        fallos++; //se suma un fallo
    }

    Semaforo propio_a_cero(0); //semaforo nuestro sin ningun permiso
    if (propio_a_cero.tryAcquireFor(std::chrono::milliseconds(50))) { //espera 50 ms y debe devolver false, si devuelve true falla
        std::cout << "FALLA: Semaforo con valor 0 devolvio true en tryAcquireFor(50 ms)\n"; //dice que comprobacion ha fallado
        fallos++; //se suma un fallo
    }

    std::counting_semaphore<valor_semaforo> estandar_a_cero(0); //semaforo estandar sin ningun permiso
    if (estandar_a_cero.try_acquire_for(std::chrono::milliseconds(50))) { //lo mismo con el estandar
        std::cout << "FALLA: std::counting_semaphore con valor 0 devolvio true en try_acquire_for(50 ms)\n"; //dice que comprobacion ha fallado
        fallos++; //se suma un fallo
    }

    if (fallos == 0) { //si no ha fallado ninguna comprobacion
        std::cout << "OK\n"; //imprime OK
        return 0; //el programa termina bien
    }
    return 1; //hubo algun fallo, termina con 1
}
