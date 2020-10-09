/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __CONTROL_INSTRUCTIONS__
#define __CONTROL_INSTRUCTIONS__

#include "instruction.h"

struct BranchInstruction : DecodedInstruction
{
    int destination;
    BranchInstruction(Branch *branch, int destination);

    virtual bool take(Cpu *cpu);

    void execute(Cpu *cpu);

    std::string __str__();
};

struct Bne : BranchInstruction
{
    int leftIndex;
    int rightIndex;
    Bne(Branch *branch, int destination);

    bool take(Cpu *cpu);

    std::string __str__();
};

#endif
