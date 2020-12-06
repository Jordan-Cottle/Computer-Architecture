/*
    Author: Jordan Cottle
    Created: 10/29/2020
*/

#include <assert.h>

#include "program_counter.h"

ProgramCounter::ProgramCounter()
{
    this->value = 0;
    this->step = 1;
}

ProgramCounter::ProgramCounter(uint32_t memoryOffset)
{
    this->value = 0;
    this->step = memoryOffset;
}

ProgramCounter ProgramCounter::operator++()
{
    this->value += this->step;
    return *this;
}

bool ProgramCounter::operator==(uint32_t value)
{
    return this->value == value;
}

void ProgramCounter::jump(uint32_t offset)
{
    assert(offset % this->step == 0);
    DEBUG << "Jumping a distance of " << str(offset) << "\n";
    this->value += offset;
}

std::string ProgramCounter::__str__()
{
    return "PC: " + str(this->value);
}
