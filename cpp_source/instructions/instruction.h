
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include <vector>

#include "misc.h"

struct Instruction : printable
{
    std::string operation;
    std::vector<std::string> arguments;

    Instruction(std::string, std::vector<std::string>);

    std::string __str__();
};

#endif
