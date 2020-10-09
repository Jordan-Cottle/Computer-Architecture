/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __SIMULATION_OBJECT__
#define __SIMULATION_OBJECT__

#include "enumerated_object.h"

struct Event;
struct EventQueue;

struct SimulationDevice : printable
{

    ulong clocksProcessed;
    int eventsProcessed;

    std::string type;

    SimulationDevice(std::string);

    virtual void tick(ulong time, EventQueue *masterEventQueue);
    virtual void process(Event *event, EventQueue *masterEventQueue);

    std::string __str__();
};

struct UnrecognizedEvent : std::runtime_error
{
    UnrecognizedEvent(std::string);
};

#endif
