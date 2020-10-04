/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __SIMULATION_OBJECT__
#define __SIMULATION_OBJECT__

#include "enumerated_object.h"

struct Event;
struct EventQueue;

struct SimulationDevice : EnumeratedObject
{

    int eventsProcessed;

    SimulationDevice(std::string);

    virtual void process(Event *, EventQueue *) = 0;

    std::string __str__();
};

struct UnrecognizedEvent : std::runtime_error
{
    UnrecognizedEvent(std::string);
};

#endif
