#ifndef EVENT_H
#define EVENT_H

#include "enumerated_object.h"

struct Event : EnumeratedObject
{
    int time;
    int value;
    Event(int time, int value);

    bool operator<(const Event &);

    Event *process();

    virtual std::string __str__();
};

#endif