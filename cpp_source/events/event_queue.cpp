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

void EventQueue::tick(ulong time)
{
    while (!this->empty() && this->nextTime() == time)
    {
        Event *event = this->pop();
        SimulationDevice *device = event->device;

        std::cout << "Processing " << event << "\n";
        device->process(event, this);
        std::cout << device << "\n";
    }
}

std::string EventQueue::__str__()
{
    return str(this->events);
}