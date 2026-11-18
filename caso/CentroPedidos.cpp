#include "CentroPedidos.h"
#include "Config.h"       
#include "Estadisticas.h"  
#include <chrono>  
#include <thread> 

//CONSTRUCTOR
CentroPedidos::CentroPedidos(const Contexto& ctx, int existencias)
    : _ctx(ctx),
      _carrera(ctx.config.fallo("carrera")),  // ¿se ha pasado --fallo carrera?
      _existencias(6, existencias),           // 6 productos, todos con las mismas unidades
      _huecos(8),                             // la cola empieza con 8 sitios libres
      _elementos(0) {}                        // y con 0 pedidos dentro



//INVENTARIO 
int CentroPedidos::consultar(int producto) {
    std::shared_lock<std::shared_mutex> lk(_mInventario);  // candado de lectura (pueden entrar varios)
    return _existencias[producto];                         // devuelvo cuántas quedan
}                                                          // aquí se suelta el candado solo

bool CentroPedidos::reservar(int producto) {
    if (_carrera) {                                            // MODO FALLO: error a propósito
        if (consultar(producto) <= 0) {                        // miro si hay (1ª sección crítica)
            return false;                                      // no hay: no reservo
        }                                                      // aquí ya se ha soltado el candado
        std::this_thread::yield();                             // cedo el turno para que se cuele otro
        std::unique_lock<std::shared_mutex> lk(_mInventario);  // cojo el candado otra vez (2ª sección crítica)
        _existencias[producto]--;                              // resto SIN volver a mirar -> puede quedar en -1
        comprobarExistencias(producto);                        // compruebo si ha quedado en negativo
        return true;
}                                        

std::unique_lock<std::shared_mutex> lk(_mInventario);  // VERSIÓN BUENA: candado de escritura
    if (_existencias[producto] <= 0) {                     // miro si hay...
        return false;                                      // no hay: no reservo
    }
    _existencias[producto]--;                              // ...y resto sin soltar el candado
    comprobarExistencias(producto);                        // compruebo el invariante
    return true;                                           // reservado
}                                                          // aquí se suelta el candado solo

void CentroPedidos::reponer() {
    std::unique_lock<std::shared_mutex> lk(_mInventario);  // candado de escritura
    for (int p = 0; p < 6; ++p) {                          // recorro los 6 productos
        _existencias[p]++;                                 // sumo 1 a cada uno
    }
}                                                          // aquí se suelta el candado solo

void CentroPedidos::comprobarExistencias(int producto) {
    if (_existencias[producto] < 0) {                     // si ha quedado en negativo...
        _ctx.estadisticas.violacion("existencias < 0");   // ...lo apunto como error
    }
}
