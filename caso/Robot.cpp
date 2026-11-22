#include "Robot.h"
#include "Estadisticas.h"
#include "Traza.h"
#include "Vigilante.h"
#include <chrono>
#include <string>
#include <thread>

Robot::Robot(int id, const Contexto& ctx, Pasillo& pasillo, CentroPedidos& centro):     //// Define el constructor de robot
    _id(id),        //Define el identificador de robot
    _ctx(ctx),      //el contexto (estadisticas, traza, vigilante)
    _pasillo(pasillo),       //Define la referencia al pasillo compartido que usará el robot
    _centro(centro) {}      //el centro de pedidos


void Robot::ejecutar() {
    const std::string yo = "robot " + std::to_string(_id);     // para la traza

    while (true) {
        // a. saco un pedido, si es -1 es el de fin y me voy
        int producto = _centro.sacarPedido();
        if (producto == -1) {
            break;
        }

        // b. entro al pasillo por la celda 0
        _pasillo.entrar(_id);
        int celda = 0;
        _ctx.traza.evento(yo, "entra al pasillo a por el producto " + std::to_string(producto));

        // c. vuelta entera: 12 movimientos hasta volver a la celda 0
        for (int paso = 0; paso < Pasillo::CELDAS; paso++) {
            int siguiente = (celda + 1) % Pasillo::CELDAS;
            _pasillo.ocupar(_id, siguiente);        // primero ocupo la siguiente
            std::this_thread::sleep_for(std::chrono::milliseconds(1));     // el movimiento tarda 1 ms
            _pasillo.soltar(_id, celda);            // y luego suelto la que tenia
            celda = siguiente;
            _bateria--;
            _ctx.estadisticas.sumar("movimientos", 1);

            if (celda == 2 * producto + 1) {        // el producto p esta en la celda 2p+1
                _ctx.traza.evento(yo, "coge el producto en la celda " + std::to_string(celda));
            }
        }

        // d. dejo el producto (si el buffer esta lleno espero aqui, en la celda 0) y salgo
        _centro.dejarProducto(producto);
        _pasillo.salir(_id);
        _ctx.estadisticas.sumar("entregas", 1);
        _ctx.vigilante.progreso();
        _ctx.traza.evento(yo, "entrega y sale del pasillo");

        // e. si no me llega la bateria para otra vuelta, cargo
        if (_bateria < Pasillo::CELDAS) {
            _pasillo.ocuparCargador(_id);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));     // cargar tarda 5 ms
            _bateria = 30;
            _pasillo.soltarCargador(_id);
            _ctx.traza.evento(yo, "ha cargado la bateria");
        }
    }
    _ctx.traza.evento(yo, "termina");
}