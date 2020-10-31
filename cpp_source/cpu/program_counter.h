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
    int value;

    ProgramCounter();
    ProgramCounter(int memoryOffset);

    ProgramCounter operator++();
    bool operator==(int value);

    void jump(int destination);

    std::string __str__();
};

#endif
