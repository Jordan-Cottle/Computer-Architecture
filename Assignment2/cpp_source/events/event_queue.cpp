#include "event.h"
#include "event_queue.h"

void EventQueue::push(Event *event)
{
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

std::string EventQueue::__str__()
{
    return str(this->events);
}