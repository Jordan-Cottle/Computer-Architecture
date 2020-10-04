
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

    int nextTime();

    std::string __str__();
};

#endif