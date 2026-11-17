#pragma once 
#include "Contexto.h"
#include "Semaforo.h"
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <vector>

#include <chrono>
#include <queue>
#include <thread>

class CentroPedidos {
public:
    // existencias = con cuántas unidades empieza cada producto
CentroPedidos(const Contexto& ctx, int existencias);
    //INVENTARIO
    int consultar(int producto); // dice cuántas quedan de un producto
    bool reservar(int producto);// quita una si hay (true) o no hace nada (false)
    void reponer();// suma 1 a cada producto

    // COLA DE PEDIDOS
    void meterPedido(int producto); // deja un pedido (espera si está llena)
    int sacarPedido(); // coge el pedido más antiguo (espera si está vacía)
    void meterFin();// deja un pedido -1 para que un robot termine(-1 sera nuestro valor de fin)


    //HILOS
    void bucleGenerador(int g, int pedidos);//intenta crear pedidos y meterlos en la cola
    void bucleGestor();// cada 5 ms repone 1 unidad de cada producto
    void avisarGestor();//al final para que el gestor pare

    // EMPAQUETADO
    void dejarProducto(int producto); // robot deja un producto en el buffer
    int cogerProducto();              // empaquetador saca un producto
    void cerrarEmpaquetado();         // main cierra el buffer al terminar robots
    void bucleEmpaquetador(int id);   // hilo empaquetador

private:
    void comprobarExistencias(int producto);// avisa si un producto queda en negativo

    const Contexto&_ctx;//guardas estadisitcas 
    bool _carrera;// true si se usa --fallo carrera

    // INVENTARIO
    std::shared_mutex _mInventario;// candado del inventario
    std::vector<int> _existencias;// cuántas unidades quedan de cada producto
    
    //COLA DE PEDIDOS(8 CAPACIDAD)
    Semaforo _huecos;// sitios libres (empieza en 8)
    std::counting_semaphore<8> _elementos;// pedidos que hay (empieza en 0)
    std::mutex _mCola; // candado de la cola
    std::vector<int> _cola;// los pedidos

    //GESTOR
    std::mutex _mGestor;// candado del gestor
    std::condition_variable _cvGestor;// timbre para despertar al gestor
    bool _finGestor = false;// true = el gestor tiene que terminar

    // EMPAQUETADO
    std::mutex mutexEmpaquetado;
    std::condition_variable EmpaquetadoLleno;
    std::condition_variable EmpaquetadoVacio;
    std::queue<int> bufferEmpaquetado;
    bool empaquetadoCerrado = false;


};


