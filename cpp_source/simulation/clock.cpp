/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "clock.h"

Clock::Clock()
{
    this->cycle = 0;
}

ulong Clock::tick()
{
    return ++this->cycle;
}

std::string Clock::__str__()
{
    return "Clock: " + str(this->cycle);
}
