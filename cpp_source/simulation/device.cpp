/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "event.h"
#include "device.h"

SimulationDevice::SimulationDevice(std::string name)
{
    this->clocksProcessed = 0;
    this->eventsProcessed = 0;
}

void SimulationDevice::tick()
{
    this->clocksProcessed += 1;
}

void SimulationDevice::process(Event *event)
{
    if (!event->handled)
    {
        throw UnrecognizedEvent(event->type);
    }

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