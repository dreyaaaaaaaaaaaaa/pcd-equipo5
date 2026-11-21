#include <chrono>
#include <mutex>
#include "Estadisticas.h"  

void Pasillo::ocuparCargador(int robot){        //Demomento no vamos a usar el identificador ya que mas adelante nos servira para
                                                //escribir una traza que indique que el robot ha pedido un cargador
    (void)robot;        //Todavia no usamos robot                        

    const auto inicio = std::chrono::steady_clock::now();       //Se gurda donde empieza la espera antes de pedir el mutex para ver cuanto tiempo tarda tambien el robot esperando su turno
    
    std::unique_lock<std::mutex> lock(_mCargadores);        //Bloque el mutex para proteger los datos compartidos de cargadores
   
    const long miTicket = _ticketSiguiente++;       //Cada robot recibe un numero y el contador aumenta ahsta que el robot reciba otro ticket

    _cvCargadores.wait(lock, [this, miTicket]{      //Robot espera hasta que su ticket sea el siguiente y que haya un cargador libre wait libera el mutex y lo vuelve a bloquear 
        return miTicket == _ticketAtendido && _cargadoresLibres > 0;        // Puede continuar si su ticket es el turno actual y hay un cargador libre.
    });

    --_cargadoresLibres;      // El robot tiene turno y plaza, le resamos esa para que salga ocupada
    ++_ticketAtendido;      // Avanzamos un turno, el siguiente ticket puede ocupar plaza si esta libre
    lock.unlock();      //Libera el mutex
    _cvCargadores.notify_all();     // Todos los robots vuelven a comprobar si les toca y si hay plaza libre.

    const auto fin = std::chrono::steady_clock::now();      //Para medir cuanto tiempo pasa desde que el robot llega hasta que consigue plaza
    const auto espera = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio);        // Calcula cuánto tiempo esperó el robot en milisegundos.

}


void Pasillo::soltarCargador(int robot){        //Demomento no vamos a usar el identificador ya que mas adelante nos servira para
                                                //escribir una traza que indique que el robot ha soltado un cargador
    (void)robot;{       

        std::lock_guard<std::mutex> lock(_mCargadores);     //// Bloquea el mutex mientras se actualizan los datos de los cargadores.
        ++_cargadoresLibres;}       // Suma uno al número de cargadores libres porque el robot libera su plaza.

        _cvCargadores.notify_all();}        // Avisa a los robots que esperan para que comprueben si ya pueden ocupar un cargador.
    
    