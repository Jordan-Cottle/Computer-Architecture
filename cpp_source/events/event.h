#ifndef EVENT_H
#define EVENT_H

#include "enumerated_object.h"
#include "device.h"

struct Event : EnumeratedObject
{
    bool handled = false;
    int time;
    SimulationDevice *device;

    Event(int time, SimulationDevice *device);
    Event(std::string type, int time, SimulationDevice *device);

    bool operator<(const Event &);

    virtual std::string __str__();
};

#endif