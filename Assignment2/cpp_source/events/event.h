#ifndef EVENT_H
#define EVENT_H

#include "enumerated_object.h"
#include "device.h"

struct Event : EnumeratedObject
{
    int time;
    SimulationDevice *device;

    Event(int, SimulationDevice *);
    Event(std::string, int, SimulationDevice *);

    bool operator<(const Event &);

    virtual std::string __str__();
};

#endif