#include "event.h"
#include "heap.h"
#include "misc.h"

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

struct EventQueue : printable
{
    MinHeap<Event *> events;

    void push(Event *event);

    Event *pop();

    Event *top();

    bool empty();

    std::string __str__();
};

#endif