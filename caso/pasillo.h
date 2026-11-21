#pragma once 
#include "Contexto.h"
#include "array"
#include <condition_variable>
#include <mutex>

class Pasillo {
public:

Pasillo(const Contexto& ctx, int cargadores);
        //CARGADORES    
    void ocuparCargador(int robot);     // Espera hasta que el robot pueda ocupar un cargador.
    void soltarCargador(int robot);     // Libera el cargador que estaba usando el robot.



private:

    const Contexto&_ctx;       //Guardas estadisitcas 
    std::mutex _mCargadores;     // Protege los datos compartidos de los cargadores.
    std::condition_variable _cvCargadores;       // Permite que los robots esperen hasta que haya un cargador disponible.
    int _cargadoresLibres = 2;       // Número de cargadores disponibles
    long _ticketsSiguiente = 0;       // Ticket siguiente a asignar
    long _ticketAtendido = 0;       // Ticket que se está atendiendo








}