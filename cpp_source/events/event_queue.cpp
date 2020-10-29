#include "event.h"
#include "event_queue.h"

#include "simulation.h"
using namespace Simulation;

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

        device->process(event);
    }
}

void EventQueue::flush(ulong time, SimulationDevice *device)
{
    if (this->empty())
    {
        return; // No events, nothing to do
    }

    if (time < simulationClock.cycle)
    {
        throw std::runtime_error("Attempted to flush events in the past!");
    }

    // TODO: Structure EventQueue better to make this better

    std::vector<Event *> events = std::vector<Event *>();

    // Skip to time requested
    while (!this->empty() && this->nextTime() < time)
    {
        events.push_back(this->pop());
    }

    // Filter out events with the specified event
    while (!this->empty() && this->nextTime() == time)
    {
        Event *event = this->pop();

        if (event->device != device)
        {
            events.push_back(event);
        }
    }

    // Put events back in their place
    for (auto event : events)
    {
        this->push(event);
    }
}

std::string EventQueue::__str__()
{
    return str(this->events);
}