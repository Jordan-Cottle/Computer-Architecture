/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __ARITHMETIC_INSTRUCTION__
#define __ARITHMETIC_INSTRUCTION__

#include "instruction.h"

struct ArithmeticInstruction : DecodedInstruction
{
    int destinationIndex;

    int leftIndex;
    int rightIndex;
    bool immediate;
    int immediateValue;

    ArithmeticInstruction(Instruction *instruction);

    ArithmeticInstruction(Instruction *instruction, int immediateValue);

    virtual void execute(Cpu *cpu) = 0;
};

struct Add : ArithmeticInstruction
{
    Add(Instruction *instruction);
    Add(Instruction *instruction, int immediateValue);

    void execute(Cpu *cpu);

    std::string __str__();
};

#endif
