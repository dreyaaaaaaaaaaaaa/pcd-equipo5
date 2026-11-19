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



//PEDIDOS
void CentroPedidos::meterPedido(int producto) {
    _huecos.acquire();                           // cojo un sitio libre (si no hay, espero)
    {
        std::lock_guard<std::mutex> lk(_mCola);  // candado de la cola
        _cola.push_back(producto);               // meto el pedido al final
    }                                            // aquí se suelta el candado solo
    _elementos.release();                        // aviso: hay un pedido más
}


int CentroPedidos::sacarPedido() {
    auto inicio = std::chrono::steady_clock::now();  // apunto la hora para medir la espera
    _elementos.acquire();                            // cojo un pedido (si no hay, espero)
    _ctx.estadisticas.registrarEspera("robot (pedido)", std::chrono::steady_clock::now() - inicio);  // apunto lo que he esperado

    int producto;                                    // aquí guardaré el pedido
    {
        std::lock_guard<std::mutex> lk(_mCola);      // candado de la cola
        producto = _cola[0];                         // cojo el primero (el más antiguo)
        _cola.erase(_cola.begin());                  // y lo quito de la cola
    }                                                // aquí se suelta el candado solo
    _huecos.release();                               // aviso: queda un sitio libre
    return producto;                                 // devuelvo el producto (o -1 si es el de fin)
}


void CentroPedidos::meterFin() {
    meterPedido(-1);  // meto un -1: el robot que lo saque termina
}


// HILO GENERADOR

void CentroPedidos::bucleGenerador(int g, int pedidos) {
    for (int k = 0; k < pedidos; ++k) {                                // repito para cada pedido
        int producto = (7 * g + k) % 6;                                // qué producto toca (fórmula del enunciado)
        if (reservar(producto)) {                                      // si he podido coger una unidad...
            meterPedido(producto);                                     // ...meto el pedido en la cola
            _ctx.estadisticas.sumar("pedidos creados", 1);             // y lo cuento
        } else {                                                       // si no había unidades...
            std::this_thread::sleep_for(std::chrono::milliseconds(1));  // ...espero 1 ms y paso al siguiente
        }
    }
}





















//EMPAQUETADO
// Un robot deja un producto en la zona de empaquetado.
// Si ya hay cuatro productos esperando, espera hasta que
// un empaquetador saque uno y quede sitio libre
void CentroPedidos::dejarProducto(int producto) {
    std::unique_lock<std::mutex> candado(candadoEmpaquetado);


    hayHueco.wait(candado, [this] {
        return productosPendientes.size() < 4;
    });


    productosPendientes.push(producto);


    // Avisamos a un empaquetador de que ya hay un producto disponible
    hayProducto.notify_one();
}




// Un empaquetador coge el producto más antiguo que haya en el buffer
// Si no hay productos, espera. Si el buffer está cerrado y vacío,
// devuelve -1 para indicar que puede terminar
int CentroPedidos::cogerProducto() {
    std::unique_lock<std::mutex> candado(candadoEmpaquetado);


    hayProducto.wait(candado, [this] {
        return !productosPendientes.empty() || noHayMasProductos;
    });


    // El buffer se ha cerrado y ya no queda nada por empaquetar
    if (productosPendientes.empty()) {
        return -1;
    }


    int producto = productosPendientes.front();
    productosPendientes.pop();


    // Avisamos a un robot de que ahora hay una plaza libre
    hayHueco.notify_one();


    return producto;
}

// Se llama cuando ya no van a llegar más productos
// Despierta a todos los empaquetadores para que puedan revisar
// si todavía queda algo por empaquetar
void CentroPedidos::cerrarEmpaquetado() {
    {
        std::lock_guard<std::mutex> candado(candadoEmpaquetado);
        noHayMasProductos = true;
    }

    hayProducto.notify_all();
}

// Ciclo de un empaquetador: coge productos mientras queden,
// los empaqueta durante dos milisegundos y termina cuando
// el buffer está cerrado y vacío
void CentroPedidos::bucleEmpaquetador(int id) {
    // El identificador no se usa todavía, pero lo mantenemos
    // por si más adelante hace falta distinguir empaquetadores
    (void)id;

    while (true) {
        int producto = cogerProducto();

        // Fin: el buffer está cerrado y no queda ningún producto
        if (producto == -1) {
            break;
        }

        // El empaquetador tarda dos milisegundos en preparar cada paquete
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        ctx.estadisticas.incrementar("paquetes");
        ctx.vigilante.progreso();
    }
}
