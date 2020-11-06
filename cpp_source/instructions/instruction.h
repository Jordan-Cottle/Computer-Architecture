
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include <vector>

#include "misc.h"

struct Cpu;

struct RawInstruction : printable
{
    uint32_t data;

    RawInstruction(uint32_t data);

    std::string keyword();

    uint32_t opcode();

    std::string __str__();
};

struct DecodedInstruction : RawInstruction
{
    bool isFp;

    DecodedInstruction(RawInstruction *instruction);

    virtual void execute(Cpu *cpu) = 0;

    std::string __str__();
};

#endif
