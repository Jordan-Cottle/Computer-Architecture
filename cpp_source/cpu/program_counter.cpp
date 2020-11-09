/*
    Author: Jordan Cottle
    Created: 10/29/2020
*/

#include <assert.h>
#include <iostream>

#include "program_counter.h"

ProgramCounter::ProgramCounter()
{
    this->value = 0;
    this->step = 1;
}

ProgramCounter::ProgramCounter(int memoryOffset)
{
    this->value = 0;
    this->step = memoryOffset;
}

ProgramCounter ProgramCounter::operator++()
{
    this->value += this->step;
    return *this;
}

bool ProgramCounter::operator==(int value)
{
    return this->value == value;
}

void ProgramCounter::jump(int destination)
{
    assert(destination % this->step == 0);
    std::cout << "Jumping a distance of " << str(destination) << "\n";
    this->value += destination;
}

std::string ProgramCounter::__str__()
{
    return "PC: " + str(this->value);
}
