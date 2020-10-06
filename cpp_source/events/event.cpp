#include <iostream>

#include "event.h"

Event ::Event(ulong time, SimulationDevice *device) : EnumeratedObject("Event"), device(device)
{
    this->time = time;

    std::cout << this << " created\n";
};

Event ::Event(std::string type, ulong time, SimulationDevice *device) : EnumeratedObject(type), device(device)
{
    this->time = time;

    std::cout << this << " created\n";
};

std::string Event::__str__()
{
    return this->device->type + " " + this->type + " " + str(this->id) + " T:" + str(this->time);
}

bool Event ::operator<(const Event &other)
{
    return this->time < other.time;
}