/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __SIMULATION_OBJECT__
#define __SIMULATION_OBJECT__

#include "enumerated_object.h"
#include "event.h"

struct SimulationDevice : EnumeratedObject
{
    int processed_count;

    SimulationDevice(std::string);

    virtual Event *process(Event *);
    std::string __str__();
};

#endif
