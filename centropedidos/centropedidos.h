#pragma once 
#include "Contexto.h"
#include "Semaforo.h"
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <vector>

class CentroPedidos {
public:
    //existencias: vector de enteros con las existencias de cada producto
CentroPedidos(const Contexto& ctx, int existencias);
    //INVENTARIO
    int consultar(int producto);
    bool reservar(int producto);
    void reponer();

    // COLA DE PEDIDOS
    void meterPedido(int producto); 
    int sacarPedido();
    void meterFin();


    //HILOS
    void bucleGenerador(int g, int pedidos);
    void bucleGestor();
    void avisarGestor();

private:
    void comprobarExistencias(int producto);

    const Contexto&_ctx;
    bool_carrera;

    // INVENTARIO
    std::shared_mutex _mInventario;
    std::vector<int> _existencias;   
    
    //COLA DE PEDIDOS(8 CAPACIDAD)
    Semaforo_huecos;
    std::counting_semaphore<8> _elementos;
    std::mutex _mCola;
    std::vector<int> _cola;

    //GESTOR
    std::mutex _mGestor;
    std::condition_variable_any _cvGestor;
    bool _finGestor = false;


};


