
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __INSTRUCTION_QUEUE__
#define __INSTRUCTION_QUEUE__

#include <queue>

#include "misc.h"
#include "instruction.h"

struct InstructionQueue : printable
{
    std::queue<Instruction> instructions;

    InstructionQueue();
    InstructionQueue(std::vector<Instruction>);

    Instruction next();

    void fetch(int);

    std::string __str__();
};

#endif
