#pragma once
#include "CentroPedidos.h"
#include "Contexto.h"
#include "Pasillo.h"

class Robot {
public:

    Robot(int id, const Contexto& ctx, Pasillo& pasillo, CentroPedidos& centro);   // Crea un robot con su numero, el contexto, el pasillo y el centro de pedidos.
    void ejecutar();        // Ejecuta el trabajo del robot en su hilo.
private:

    int _id;        // Guarda el número identificador del robot.
    int _bateria = 30;      // Guarda la batería del robot, que empieza llena en 30 movimientos.
    const Contexto& _ctx;       // estadisticas, traza y vigilante
    Pasillo& _pasillo;      // Guarda una referencia al pasillo compartido que usa el robot.
    CentroPedidos& _centro;     // de aqui saco los pedidos y aqui dejo los productos

};