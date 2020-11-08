#ifndef EVENT_H
#define EVENT_H

#include "enumerated_object.h"
#include "device.h"

enum PRIORITY
{
    LOW = 0,
    MEDIUM = 100,
    HIGH = 200,
};

struct Event : EnumeratedObject
{
    bool handled = false;
    ulong time;
    SimulationDevice *device;
    int priority;

    Event(std::string type, ulong time, SimulationDevice *device);
    Event(std::string type, ulong time, SimulationDevice *device, int priority);

    bool operator<(const Event &);

    virtual std::string __str__();
};

#endif