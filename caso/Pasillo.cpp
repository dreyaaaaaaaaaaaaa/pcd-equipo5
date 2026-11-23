#include <chrono>
#include <mutex>
#include "Estadisticas.h"
#include "Pasillo.h"
#include "Config.h"
#include <thread>
#include <string>

//CONSTRUCTOR
Pasillo::Pasillo(const Contexto& ctx, int cargadores)
    : _ctx(ctx),
    _sinLimite(ctx.config.fallo("interbloqueo") || ctx.config.fallo("livelock")),   // en estos modos no hay limite
    _cargadoresLibres(cargadores), // empiezan todos los cargadores libres
    _inanicion(ctx.config.fallo("inanicion")){   // Activa el modo de fallo "inanicion" 
    _celdas.fill(LIBRE);                // y todas las celdas vacias
}


//CELDAS
void Pasillo::marcarCelda(int robot, int celda) {
    if (_celdas[celda] != LIBRE) {          // si ya hay otro robot se rompe el invariante
        _ctx.estadisticas.violacion("celda " + std::to_string(celda) + ": entra el robot " +
                                    std::to_string(robot) + " y ya estaba el robot " +
                                    std::to_string(_celdas[celda]));
    }
    _celdas[celda] = robot;                 // apunto que la celda es mia
}

void Pasillo::entrar(int robot) {
    std::unique_lock<std::mutex> lk(_m);
    const long miTurno = _turnoSiguiente++; // cojo numero, se entra por orden de llegada
    _cvEntrada.wait(lk, [this, miTurno] {
        if (miTurno != _turnoActual || _celdas[0] != LIBRE) {
            return false;                         // no es mi turno o la entrada esta ocupada
        }
        if (_sinLimite) {
            return true;                          // MODOS DE FALLO: entro aunque esten los 12 dentro
        }
        return _dentro < MAX_DENTRO               // hay sitio (como mucho 11 dentro)
            && _celdas[CELDAS - 1] == LIBRE;      // y no viene nadie por la 11 (el de dentro tiene prioridad)
    });
    marcarCelda(robot, 0);
    _dentro++;
    _turnoActual++;                         // paso el turno al siguiente
    _cvEntrada.notify_all();                // para que el del siguiente turno lo compruebe
}

void Pasillo::ocupar(int robot, int celda) {
    const auto inicio = std::chrono::steady_clock::now();      // para medir cuanto espero
    std::unique_lock<std::mutex> lk(_m);    // unique_lock porque wait tiene que poder soltarlo
    _cvCelda[celda].wait(lk, [this, celda] {    // espero mientras la celda este ocupada
        return _celdas[celda] == LIBRE;
    });
    marcarCelda(robot, celda);
    _ctx.estadisticas.registrarEspera("robot (celda siguiente)", 
    std::chrono::steady_clock::now() - inicio);
}

void Pasillo::soltar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[celda] == robot) {          // solo la libero si de verdad es mia
        _celdas[celda] = LIBRE;
    }
    _cvCelda[celda].notify_one();           // aviso al robot de detras, que es el unico que puede querer esta celda
        if (celda == 0 || celda == CELDAS - 1) {
        _cvEntrada.notify_all();            // la 0 o la 11 han quedado libres, a lo mejor alguien puede entrar
    }
}

bool Pasillo::intentarOcupar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[celda] != LIBRE) {
        return false;                       // ocupada: no espero, le digo al robot que no ha podido
    }
    marcarCelda(robot, celda);
    return true;
}

void Pasillo::salir(int robot) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[0] == robot) {
        _celdas[0] = LIBRE;
    }
    _dentro--;                              // un robot menos dentro
    _cvCelda[0].notify_one();               // aviso al robot de la celda 11, que quiere la 0
    _cvEntrada.notify_all();                // hay sitio y la celda 0 esta libre
}


void Pasillo::ocuparCargador(int robot)        //Demomento no vamos a usar el identificador ya que mas adelante nos servira para
{                                              //escribir una traza que indique que el robot ha pedido un cargador

    const auto inicio = std::chrono::steady_clock::now();      //Se gurda donde empieza la espera antes de pedir el mutex para ver cuanto tiempo tarda tambien el robot esperando su turno
    {
    std::unique_lock<std::mutex> lock(_mCargadores);        //Bloque el mutex para proteger los datos compartidos de cargadores
    const long miTicket = _ticketSiguiente++;       //Cada robot recibe un numero y el contador aumenta ahsta que el robot reciba otro ticket

    if (_inanicion){        //Si está activado el modo de fallo "inanicion", añade el robot al conjunto de robots que esperan un cargador.
        _robotsEsperando.insert(robot);        // Añade el ID del robot al conjunto de robots que esperan un cargador.
    }
    _cvCargadores.wait(lock, [this, miTicket, robot]{      //Robot espera hasta que su ticket sea el siguiente y que haya un cargador libre wait libera el mutex y lo vuelve a bloquear 
        if (_cargadoresLlibres == 0) {       //Si no hay cargadores libres, el robot se queda esperando
            return false;  
        }
        if (_inanicion) {        //Si el modo de fallo "inanicion" esta activado, se comprueba si hay robots con tickets menores que el del robot actual
            return !_robotsEsperando.empty()        // Comprueba que haya robots esperando antes de consultar el primero.
                && *_robotsEsperando.begin() == robot;     // Comprueba que el robot con el ID más bajo sea este robot.
        }
        return miTicket == _ticketAtendido;       // En el modo normal, comprueba si este robot tiene el siguiente turno FIFO.
    });

    --_cargadoresLibres;      // El robot tiene turno y plaza, le resamos esa para que salga ocupada

    if (_inanicion) {       //Si está activado el modo de fallo "inanicion", añade el robot al conjunto de robots que esperan un cargador.
        _robotsEsperando.erase(robot);      // Quita al robot del conjunto porque ya ha conseguido un cargador.
    }else{
         ++_ticketAtendido;  // Avanzamos un turno, el siguiente ticket puede ocupar plaza si esta libre
    }
 }
    _cvCargadores.notify_all();     // Todos los robots vuelven a comprobar si les toca y si hay plaza libre.

    const auto fin = std::chrono::steady_clock::now();      //Para medir cuanto tiempo pasa desde que el robot llega hasta que consigue plaza
    _ctx.estadisticas.registrarEspera("robot (cargador)", fin - inicio);        // Guarda en las estadísticas cuánto esperó el robot para conseguir un cargador.

}


void Pasillo::soltarCargador(int robot){        //Demomento no vamos a usar el identificador ya que mas adelante nos servira para
                                                //escribir una traza que indique que el robot ha soltado un cargador
    (void)robot;{       

        std::lock_guard<std::mutex> lock(_mCargadores);     //// Bloquea el mutex mientras se actualizan los datos de los cargadores.
        ++_cargadoresLibres;}       // Suma uno al número de cargadores libres porque el robot libera su plaza.

        _cvCargadores.notify_all();}        // Avisa a los robots que esperan para que comprueben si ya pueden ocupar un cargador.
