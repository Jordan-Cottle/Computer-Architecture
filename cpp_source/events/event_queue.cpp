#include "event.h"
#include "event_queue.h"

void EventQueue::push(Event *event)
{
    if (event == NULL)
    {
        return;
    }

    this->events.push(event);
}

Event *EventQueue::pop()
{
    return this->events.pop();
}

Event *EventQueue::top()
{
    return this->events.top();
}

bool EventQueue::empty()
{
    return this->events.empty();
}

ulong EventQueue::nextTime()
{
    return this->top()->time;
}

std::string EventQueue::__str__()
{
    return str(this->events);
}