#include <iostream>

#include "event.h"

Event ::Event(int time, int value) : EnumeratedObject("Event")
{
    this->time = time;
    this->value = value;

    std::cout << this << " created\n";
};

std::string Event::__str__()
{
    return "Event " + std::to_string(this->id) + " (T:" + std::to_string(this->time) + " V:" + std::to_string(this->value) + ")";
}

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
