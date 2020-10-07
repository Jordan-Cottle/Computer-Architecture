/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#include "program.h"
Program::Program(std::vector<Instruction *> instructions, std::unordered_map<std::string, int> labels)
{
    this->instructions = instructions;
    this->labels = labels;
}

Instruction *Program::line(int index)
{
    return this->instructions.at(index);
}

int Program::index(std::string label)
{
    return this->labels[label];
}

std::string Program::__str__()
{
    std::vector<std::string> s = {};

    for (auto instruction : this->instructions)
    {
        s.push_back(str(instruction));
    }

    for (std::pair<std::string, int> element : this->labels)
    {
        std::string instruction = s[element.second];

        instruction = element.first + "::\t" + instruction;

        s[element.second] = instruction;
    }

    std::string output = "~~~~~~~~~~~Program~~~~~~~~~~~\n";

    for (auto line : s)
    {
        if (line.find("::") == std::string::npos)
        {
            output += "\t";
        }
        output += line + "\n";
    }

    output += "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    return output;
}
