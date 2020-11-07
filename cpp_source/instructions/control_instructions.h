/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __CONTROL_INSTRUCTIONS__
#define __CONTROL_INSTRUCTIONS__

#include "instruction.h"

struct ControlInstruction : DecodedInstruction
{
    int destination;
    ControlInstruction(RawInstruction *instruction);

    virtual bool take(Cpu *cpu);

    void execute(Cpu *cpu);

    std::string __str__();
};

struct BranchInstruction : ControlInstruction
{
    int leftIndex;
    int rightIndex;
    BranchInstruction(RawInstruction *instruction);
};

struct Bne : BranchInstruction
{
    Bne(RawInstruction *instruction);

    bool take(Cpu *cpu);

    std::string __str__();
};

struct Jump : ControlInstruction
{
    int registerIndex;
    Jump(RawInstruction *instruction);

    void execute(Cpu *cpu);
};

#endif
