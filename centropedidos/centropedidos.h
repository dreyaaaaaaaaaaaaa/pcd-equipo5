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
    CentroPedidos(const Contexto& ctx, int existencias);

    int consultar(int producto);
    bool reservar(int producto);
    void reponer();
