#include <iostream>

#include "event.h"

Event ::Event(ulong time, SimulationDevice *device) : EnumeratedObject("Event"), device(device)
{
    this->time = time;
    this->priority = 10;
};
Event ::Event(ulong time, SimulationDevice *device, int priority) : EnumeratedObject("Event"), device(device)
{
    this->time = time;
    this->priority = priority;
};

Event ::Event(std::string type, ulong time, SimulationDevice *device) : EnumeratedObject(type), device(device)
{
    this->time = time;
    this->priority = 10;
};

Event ::Event(std::string type, ulong time, SimulationDevice *device, int priority) : EnumeratedObject(type), device(device)
{
    this->time = time;
    this->priority = priority;
};

std::string Event::__str__()
{
    return this->type + " #" + str(this->id) + " T:" + str(this->time);
}

bool Event ::operator<(const Event &other)
{
    if (this->time == other.time)
    {
        return this->priority < other.priority;
    }

    return this->time < other.time;
}