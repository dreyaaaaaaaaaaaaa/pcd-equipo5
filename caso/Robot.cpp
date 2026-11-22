#pragma once
#include <chrono>
#include <thread>
#include <Robot.h>




Robot::Robot(int id, Pasillo& pasillo):     //// Define el constructor de robot
    _id(id),        //Define el identificador de robot
    _pasillo(pasillo)       //Define la referencia al pasillo compartido que usará el robot
