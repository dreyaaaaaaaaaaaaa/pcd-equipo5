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

const int num_productos = 6; //productos distintos que hay en el almacen

bool ejecutarSimulacion(const Config& config, Estadisticas& estadisticas, Traza& traza, const std::string& etiqueta) { //ejecuta una simulacion entera y devuelve true si no se ha roto ningun invariante
    int num_robots = config.entero("robots", 12); //lee --robots, si no se pasa son 12
    int cargadores = config.entero("cargadores", 2); //lee --cargadores, por defecto 2
    int existencias = config.entero("existencias", 3); //lee --existencias, por defecto 3
    int generadores = config.entero("generadores", 2); //lee --generadores, por defecto 2
    int pedidos = config.entero("pedidos", 30); //lee --pedidos, por defecto 30
    int empaquetadores = config.entero("empaquetadores", 2); //lee --empaquetadores, por defecto 2

    Vigilante vigilante(std::chrono::milliseconds(config.entero("vigilante", 2000)), estadisticas, etiqueta); //si pasan 2000 ms sin progreso el vigilante corta el programa
    Contexto ctx{config, estadisticas, traza, vigilante}; //paquete con la configuracion, las estadisticas, la traza y el vigilante

    CentroPedidos centro(ctx, existencias); //inventario, cola de pedidos y empaquetado
    Pasillo pasillo(ctx, cargadores); //el pasillo con sus cargadores
    std::vector<Robot> robots; //lista de robots
    robots.reserve(num_robots); //se reserva sitio para todos, asi no cambian de sitio al crearlos
    for (int r = 0; r < num_robots; r++) { //se crea un robot por vuelta
        robots.emplace_back(r, ctx, pasillo, centro); //el robot r, con el contexto, el pasillo y el centro de pedidos
    }

    vigilante.iniciar(); //el vigilante empieza a contar

    std::vector<std::thread> hilos_generadores; //lista de hilos de los generadores
    for (int g = 0; g < generadores; g++) { //un hilo por generador
        hilos_generadores.emplace_back(&CentroPedidos::bucleGenerador, &centro, g, pedidos); //el hilo ejecuta bucleGenerador sobre centro, con g y pedidos
    }
    std::vector<std::thread> hilos_robots; //lista de hilos de los robots
    for (int r = 0; r < num_robots; r++) { //un hilo por robot
        hilos_robots.emplace_back(&Robot::ejecutar, &robots[r]); //el hilo ejecuta ejecutar del robot r
    }
    std::vector<std::thread> hilos_empaquetadores; //lista de hilos de los empaquetadores
    for (int e = 0; e < empaquetadores; e++) { //un hilo por empaquetador
        hilos_empaquetadores.emplace_back(&CentroPedidos::bucleEmpaquetador, &centro, e); //el hilo ejecuta bucleEmpaquetador sobre centro
    }
    std::thread hilo_gestor(&CentroPedidos::bucleGestor, &centro); //un solo hilo del gestor, que repone el inventario

    for (auto& h : hilos_generadores) { //se recorren los generadores
        h.join(); //se espera a que acaben todos
    }
    for (int r = 0; r < num_robots; r++) { //una vez por cada robot
        centro.meterFin(); //se mete un pedido de fin en la cola, avisa al robot de que debe parar
    }
    for (auto& h : hilos_robots) { //se recorren los robots
        h.join(); //se espera a que acaben todos
    }
    centro.cerrarEmpaquetado(); //se avisa de que ya no llegan mas productos al empaquetado
    for (auto& h : hilos_empaquetadores) { //se recorren los empaquetadores
        h.join(); //se espera a que acaben todos
    }
    centro.avisarGestor(); //se avisa al gestor de que pare
    hilo_gestor.join(); //se espera a que acabe

    vigilante.detener(); //ya no hace falta vigilar

    for (int p = 0; p < num_productos; p++) { //se recorren los 6 productos
        if (centro.consultar(p) < 0) { //si las existencias de ese producto son negativas
            estadisticas.violacion("al final las existencias del producto " + std::to_string(p) + " son negativas"); //se apunta como invariante roto
        }
    }
    long creados = estadisticas.valor("pedidos creados"); //pedidos que se han creado
    long entregas = estadisticas.valor("entregas"); //pedidos que han entregado los robots
    long paquetes = estadisticas.valor("paquetes"); //pedidos que han empaquetado los empaquetadores
    if (entregas != creados || paquetes != creados) { //si los tres numeros no coinciden
        estadisticas.violacion("pedidos creados = " + std::to_string(creados) + ", entregas = " + std::to_string(entregas) + ", paquetes = " + std::to_string(paquetes)); //se apunta como invariante roto, con los tres numeros
    }
    return estadisticas.sinViolaciones(); //true si no se ha roto ningun invariante
}

int main(int argc, char* argv[]) //aqui empieza el programa, argc y argv son lo que se escribe en la terminal
{
    Config config(argc, argv); //lee las opciones de la terminal, como --robots 8
    Lanzador lanzador("almacen", {"robots", "cargadores", "existencias", "generadores", "pedidos", "empaquetadores"}, {"carrera", "interbloqueo", "livelock", "inanicion"}); //nombre del programa, opciones validas y modos de fallo, rechaza cualquier otra opcion
    return lanzador.ejecutar(config, ejecutarSimulacion); //ejecuta la simulacion una vez o varias con --estres, devuelve el codigo de salida
}
