//CONSTRUCTOR
Pasillo::Pasillo(const Contexto& ctx, int cargadores)
    : _ctx(ctx),
      _cargadoresLibres(cargadores) {   // empiezan todos los cargadores libres
    _celdas.fill(LIBRE);                // y todas las celdas vacias
}


//CELDAS
void Pasillo::entrar(int robot) {
    ocupar(robot, 0);       // de momento entrar es solo ocupar la celda 0
}

void Pasillo::ocupar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);     // candado de las celdas
    if (_celdas[celda] != LIBRE) {          // si ya hay otro robot se rompe el invariante
        _ctx.estadisticas.violacion("celda " + std::to_string(celda) + ": entra el robot " +
                                    std::to_string(robot) + " y ya estaba el robot " +
                                    std::to_string(_celdas[celda]));
    }
    _celdas[celda] = robot;                 // apunto que la celda es mia
}

void Pasillo::soltar(int robot, int celda) {
    std::lock_guard<std::mutex> lk(_m);
    if (_celdas[celda] == robot) {          // solo la libero si de verdad es mia
        _celdas[celda] = LIBRE;
    }
}

void Pasillo::salir(int robot) {
    soltar(robot, 0);       // salir es soltar la celda 0
}