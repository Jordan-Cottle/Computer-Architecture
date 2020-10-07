
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include <vector>

#include "misc.h"

struct Cpu;

struct Instruction : printable
{
    std::string operation;

    // Arguments can be register/memory indexes or immediate values
    std::vector<int> arguments;

    Instruction(std::string operation, std::vector<int> arguments);

    Instruction(Instruction *instruction);

    std::string __str__();
};

struct Branch : Instruction
{
    std::string label;

    Branch(std::string operation, std::vector<int> arguments, std::string label);

    std::string __str__();
};

struct DecodedInstruction : Instruction
{
    bool isFp;

    DecodedInstruction(Instruction *instruction);

    virtual void execute(Cpu *cpu) = 0;
};

#endif
