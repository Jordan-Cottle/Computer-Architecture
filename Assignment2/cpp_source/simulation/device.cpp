/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "device.h"

SimulationDevice::SimulationDevice(std::string name) : EnumeratedObject(name)
{
    this->processed_count = 0;
}

std::string SimulationDevice::__str__()
{
    return "SimDevice: " + this->type + "(" + std::to_string(this->processed_count) + ")";
}

Event *SimulationDevice::process(Event *event)
{
    Event *newEvent = event->process();
    this->processed_count++;
    return newEvent;
}