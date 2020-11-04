
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "instruction.h"

constexpr int OPCODE = 0x7F;

RawInstruction::RawInstruction(uint32_t data)
{
    this->data = data;
}

uint32_t RawInstruction::opcode()
{
    return this->data & OPCODE;
}

std::string RawInstruction::__str__()
{
    std::string s = "";
    for (int i = 0; i < 32; i++)
    {
        s = str((this->data & (1 << i)) >> i) + s;
    }

    return s;
}

Instruction::Instruction(std::string operation, std::vector<int> arguments) : RawInstruction(0)
{
    this->operation = operation;
    this->arguments = arguments;
}

Instruction::Instruction(Instruction *instruction) : RawInstruction(instruction->data)
{
    this->operation = instruction->operation;
    this->arguments = instruction->arguments;
}

std::string Instruction::__str__()
{
    std::string s = this->operation;

    for (auto arg : this->arguments)
    {
        s += " " + str(arg);
    }

    return s;
}

Branch::Branch(std::string operation, std::vector<int> arguments, std::string label) : Instruction(operation, arguments)
{
    this->label = label;
}

std::string Branch::__str__()
{
    return Instruction::__str__() + " " + this->label;
}

DecodedInstruction::DecodedInstruction(Instruction *instruction) : Instruction(instruction)
{
    this->isFp = instruction->operation[0] == 'f';
}
