#include "event.h"
#include "heap.h"

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

struct EventQueue
{
    MinHeap<Event *> events;

    void push(Event *event);

    Event *pop();

    Event *top();

    bool empty();

    void display();
};

#endif