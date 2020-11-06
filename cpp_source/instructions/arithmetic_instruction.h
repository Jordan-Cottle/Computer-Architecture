/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __ARITHMETIC_INSTRUCTION__
#define __ARITHMETIC_INSTRUCTION__

#include "instruction.h"

// R and I type instructions for math operations
struct ArithmeticInstruction : DecodedInstruction
{
    int destinationIndex;

    int leftIndex;
    int rightIndex;
    bool immediate;

    ArithmeticInstruction(RawInstruction *instruction);

    virtual void execute(Cpu *cpu) = 0;
};

struct Add : ArithmeticInstruction
{
    Add(RawInstruction *instruction);

    void execute(Cpu *cpu);

    std::string __str__();
};

#endif
