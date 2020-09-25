#include <iostream>

#include "event.h"

int event_count = 0;

std::ostream &operator<<(std::ostream &os, Event *event)
{
    return os << "Event " << event->id << " (T:" << event->time << " V:" << event->value << ")";
}

Event ::Event(int time, int value)
{
    this->time = time;
    this->value = value;
    this->id = ++event_count;

    std::cout << this << " created\n";
};

bool Event ::operator<(const Event &other)
{
    return this->time < other.time;
}

Event *Event::process()
{
    std::cout << "Processing " << this << "\n";

    Event *new_event = new Event(this->time + this->value, this->value);
    delete this;
    return new_event;
}
