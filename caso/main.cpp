#include "CentroPedidos.h"
#include "Config.h"
#include "Contexto.h"
#include "Estadisticas.h"
#include "Lanzador.h"
#include "Pasillo.h"
#include "Robot.h"
#include "Traza.h"
#include "Vigilante.h"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

const int num_productos = 6;

bool ejecutarSimulacion(const Config& config, Estadisticas& estadisticas, Traza& traza, const std::string& etiqueta) {
    int num_robots = config.entero("robots", 12);
    int cargadores = config.entero("cargadores", 2);
    int existencias = config.entero("existencias", 3);
    int generadores = config.entero("generadores", 2);
    int pedidos = config.entero("pedidos", 30);
    int empaquetadores = config.entero("empaquetadores", 2);

    Vigilante vigilante(std::chrono::milliseconds(config.entero("vigilante", 2000)), estadisticas, etiqueta);
    Contexto ctx{config, estadisticas, traza, vigilante};

    CentroPedidos centro(ctx, existencias);
    Pasillo pasillo(ctx, cargadores);
    std::vector<Robot> robots;
    robots.reserve(num_robots);
    for (int r = 0; r < num_robots; r++) {
        robots.emplace_back(r, pasillo);
    }

    vigilante.iniciar();

    std::vector<std::thread> hilos_generadores;
    for (int g = 0; g < generadores; g++) {
        hilos_generadores.emplace_back(&CentroPedidos::bucleGenerador, &centro, g, pedidos);
    }
    std::vector<std::thread> hilos_robots;
    for (int r = 0; r < num_robots; r++) {
        hilos_robots.emplace_back(&Robot::ejecutar, &robots[r]);
    }
    std::vector<std::thread> hilos_empaquetadores;
    for (int e = 0; e < empaquetadores; e++) {
        hilos_empaquetadores.emplace_back(&CentroPedidos::bucleEmpaquetador, &centro, e);
    }
    std::thread hilo_gestor(&CentroPedidos::bucleGestor, &centro);

    for (auto& h : hilos_generadores) {
        h.join();
    }
    for (int r = 0; r < num_robots; r++) {
        centro.meterFin();
    }
    for (auto& h : hilos_robots) {
        h.join();
    }
    centro.cerrarEmpaquetado();
    for (auto& h : hilos_empaquetadores) {
        h.join();
    }
    centro.avisarGestor();
    hilo_gestor.join();

    vigilante.detener();

    for (int p = 0; p < num_productos; p++) {
        if (centro.consultar(p) < 0) {
            estadisticas.violacion("al final las existencias del producto " + std::to_string(p) + " son negativas");
        }
    }
    long creados = estadisticas.valor("pedidos creados");
    long entregas = estadisticas.valor("entregas");
    long paquetes = estadisticas.valor("paquetes");
    if (entregas != creados || paquetes != creados) {
        estadisticas.violacion("pedidos creados = " + std::to_string(creados) + ", entregas = " + std::to_string(entregas) + ", paquetes = " + std::to_string(paquetes));
    }
    return estadisticas.sinViolaciones();
}

int main(int argc, char* argv[])
{
    Config config(argc, argv);
    Lanzador lanzador("almacen", {"robots", "cargadores", "existencias", "generadores", "pedidos", "empaquetadores"}, {"carrera", "interbloqueo", "livelock", "inanicion"});
    return lanzador.ejecutar(config, ejecutarSimulacion);
}
