#pragma once
#include "Pasillo.h"
#include <condition_variable>
#include <mutex>
#include <thread>




class Robot {
public:

    Robot(int id, Pasillo& pasillo);        // Crea un robot con su identificador y una referencia al pasillo que usará.
    void ejecutar();        // Ejecuta el trabajo del robot en su hilo.
private:

    
    int _id;        // Guarda el número identificador del robot.
    int _bateria = 30;      // Guarda la batería del robot, que empieza llena en 30 movimientos.
    Pasillo& _pasillo;      // Guarda una referencia al pasillo compartido que usa el robot.

}

  