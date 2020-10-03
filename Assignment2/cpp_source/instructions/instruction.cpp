
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "instruction.h"

Instruction::Instruction(std::string operation, std::vector<std::string> arguments)
{
    this->operation = operation;
    this->arguments = arguments;
}

std::string Instruction::__str__()
{
    std::string s = "Instruction: " + this->operation;

    for (auto arg : this->arguments)
    {
        s += " " + arg;
    }

    return s;
}