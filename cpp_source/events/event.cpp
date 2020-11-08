#include <iostream>

#include "event.h"

Event ::Event(std::string type, ulong time, SimulationDevice *device) : EnumeratedObject(type), device(device)
{
    this->time = time;
    this->priority = MEDIUM;
};

Event ::Event(std::string type, ulong time, SimulationDevice *device, int priority) : EnumeratedObject(type), device(device)
{
    this->time = time;
    this->priority = priority;
};

std::string Event::__str__()
{
    return this->device->type + " " + this->type + " #" + str(this->id) + " T:" + str(this->time);
}

bool Event ::operator<(const Event &other)
{
    if (this->time == other.time)
    {
        return this->priority > other.priority;
    }

    return this->time < other.time;
}