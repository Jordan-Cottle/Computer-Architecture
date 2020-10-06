/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "event.h"
#include "device.h"

SimulationDevice::SimulationDevice(std::string name) : EnumeratedObject(name)
{
    this->clocksProcessed = 0;
    this->eventsProcessed = 0;
}

void SimulationDevice::tick(ulong time, EventQueue *masterEventQueue)
{
    this->clocksProcessed += 1;
}

void SimulationDevice::process(Event *event, EventQueue *masterEventQueue)
{
    this->eventsProcessed += 1;
    delete event;
}

std::string SimulationDevice::__str__()
{
    return this->type + "(" + str(this->eventsProcessed) + ")";
}

UnrecognizedEvent::UnrecognizedEvent(std::string message) : std::runtime_error(message)
{
}