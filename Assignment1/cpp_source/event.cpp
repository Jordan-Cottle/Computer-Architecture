#include <iostream>

#include "event.h"

int event_count = 0;

Event ::Event(int time, int value)
{
    this->time = time;
    this->value = value;
    this->id = ++event_count;
};

bool Event ::operator<(const Event &other)
{
    return this->time < other.time;
}

Event *Event::process()
{
    std::cout << "T=" << this->time << " Event " << this->id << ": value = " << this->value << "\n";

    Event *new_event = new Event(this->time + this->value, this->value);
    std::cout << this->id << " --> " << new_event->id << "(" << new_event->time << ")\n";

    delete this;
    return new_event;
}

std::ostream &operator<<(std::ostream &os, Event *event)
{
    return os << "Event (id=" << event->id << ", time=" << event->time << ", value=" << event->value << ")";
}
