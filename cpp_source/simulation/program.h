/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __PROGRAM__
#define __PROGRAM__

#include <unordered_map>

#include "instruction.h"

struct Program : printable
{
    std::vector<Instruction *> instructions;
    std::unordered_map<std::string, int> labels;

    Program(std::vector<Instruction *> instructions, std::unordered_map<std::string, int> labels);
    ~Program();

    Instruction *line(int index);
    int index(std::string label);

    std::string __str__();
};

#endif
