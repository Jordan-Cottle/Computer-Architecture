
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.h"
#include "heap.h"
#include "misc.h"

struct EventQueue : printable
{
    MinHeap<Event *> events;

    void push(Event *event);

    Event *pop();

    Event *top();

    bool empty();

    uint32_t size();

    ulong nextTime();

    void tick(ulong time);

    void flush(ulong time, SimulationDevice *device);

    std::string __str__();
};

#endif