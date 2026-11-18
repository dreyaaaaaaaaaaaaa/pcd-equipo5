#include <chrono>
#include <mutex>

void pasillo:ocuparCargador(int robot){ 
    //demomento no vamos a usar el identificador ya que mas adelante nos servira para
    //escribir una traza que indique que el robot ha perdido un cargador
    (void)robot; 
    //Se gurda donde empieza la espera antes de pedir el mutex para ver cuanto tiempo tarda tambien el robot 
    //esperando su turno
    const auto inicio = std::chrono::steady_clock::now();
    //para bloquear el mutex
    std::unique_lock<std::mutex> lock(_mCargadores);
    //cada robot recibe un numero y el contador aumenta ahsta que el robot reciba otro ticket
    const long miTicket = _ticketSiguiente++;

//robot espera hasta que su ticket sea el siguiente y que haya un cargador libre
    //wait libera el mutex y lo vuelve a bloquear 
    _cvCargadores.wait(lock, [this, miTicket]{
        return miTicket == _ticketAtendido && _cargadoresLibres > 0;
    });
