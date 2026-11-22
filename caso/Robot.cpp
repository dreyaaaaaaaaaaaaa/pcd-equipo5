#pragma once
#include <chrono>
#include <thread>
#include <Robot.h>




Robot::Robot(int id, Pasillo& pasillo): 
    _id(id),    
    _pasillo(pasillo) 
