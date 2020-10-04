/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "device.h"

SimulationDevice::SimulationDevice(std::string name) : EnumeratedObject(name)
{
    this->eventsProcessed = 0;
}

std::string SimulationDevice::__str__()
{
    return this->type + "(" + str(this->eventsProcessed) + ")";
}

UnrecognizedEvent::UnrecognizedEvent(std::string message) : std::runtime_error(message)
{
}