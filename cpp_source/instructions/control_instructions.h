/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __CONTROL_INSTRUCTIONS__
#define __CONTROL_INSTRUCTIONS__

#include "instruction.h"

struct ControlInstruction : DecodedInstruction
{
    ControlInstruction(RawInstruction *instruction);

    virtual bool take(Cpu *cpu);
    virtual int offset(Cpu *cpu);

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

struct Blt : BranchInstruction
{
    Blt(RawInstruction *instruction);

    bool take(Cpu *cpu);

    std::string __str__();
};

struct Jump : ControlInstruction
{
    int registerIndex;
    Jump(RawInstruction *instruction);

    int offset(Cpu *cpu);
    void execute(Cpu *cpu);
};

struct Jalr : ControlInstruction
{
    int registerIndex;
    int sourceIndex;
    Jalr(RawInstruction *instruction);

    int offset(Cpu *cpu);
    void execute(Cpu *cpu);
};

#endif
