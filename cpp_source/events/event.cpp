#include <iostream>

#include "event.h"

Event ::Event(ulong time, SimulationDevice *device) : EnumeratedObject("Event"), device(device)
{
    this->time = time;
};

Event ::Event(std::string type, ulong time, SimulationDevice *device) : EnumeratedObject(type), device(device)
{
    this->time = time;
};

std::string Event::__str__()
{
    return this->type + " #" + str(this->id) + " T:" + str(this->time);
}

bool Event ::operator<(const Event &other)
{
    return this->time < other.time;
}