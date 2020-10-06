
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "instruction_queue.h"

InstructionQueue::InstructionQueue()
{
    this->instructions = std::queue<Instruction *>();
}

InstructionQueue::InstructionQueue(std::vector<Instruction *> instructions)
{
    this->instructions = std::queue<Instruction *>();

    for (auto instruction : instructions)
    {
        this->instructions.push(instruction);
    }
}

Instruction *InstructionQueue::next()
{
    Instruction *next = this->instructions.front();

    this->instructions.pop();

    return next;
}

void InstructionQueue::fetch(int n)
{
    // TODO: Fetch new instructions from somewhere
    this->instructions.push(new Instruction("FADD", {"F1", "F2", "F3"}));
    this->instructions.push(new Instruction("FSUB", {"F2", "F3", "F4"}));
    this->instructions.push(new Instruction("FMUL", {"F3", "F4", "F5"}));
    this->instructions.push(new Instruction("FDIV", {"F4", "F5", "F6"}));
}

std::string InstructionQueue::__str__()
{
    std::string s = "InstructionQueue: {";

    std::queue<Instruction *> copy = std::queue<Instruction *>(this->instructions);

    while (!copy.empty())
    {
        s += "\n\t" + str(copy.front());
        copy.pop();
    }

    s += "\n}";

    return s;
}