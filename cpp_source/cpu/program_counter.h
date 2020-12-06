/*
    Author: Jordan Cottle
    Created: 10/29/2020
*/

#ifndef __PROGRAM_COUNTER__
#define __PROGRAM_COUNTER__

#include "misc.h"

struct ProgramCounter : printable
{

    int step;
    uint32_t value;

    ProgramCounter();
    ProgramCounter(uint32_t memoryOffset);

    ProgramCounter operator++();
    bool operator==(uint32_t value);

    void jump(uint32_t offset);

    std::string __str__();
};

#endif
