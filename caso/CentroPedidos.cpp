#include "CentroPedidos.h"
#include "Config.h"       
#include "Estadisticas.h"  
#include <chrono>  
#include <thread> 

CentroPedidos::CentroPedidos(const Contexto& ctx, int existencias)
    : _ctx(ctx),
      _carrera(ctx.config.fallo("carrera")),  // ¿se ha pasado --fallo carrera?
      _existencias(6, existencias),           // 6 productos, todos con las mismas unidades
      _huecos(8),                             // la cola empieza con 8 sitios libres
      _elementos(0) {}                        // y con 0 pedidos dentro