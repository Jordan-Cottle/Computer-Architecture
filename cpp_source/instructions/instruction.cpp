
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "instruction.h"

Instruction::Instruction(std::string operation, std::vector<int> arguments)
{
    this->operation = operation;
    this->arguments = arguments;
}

Instruction::Instruction(Instruction *instruction)
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
