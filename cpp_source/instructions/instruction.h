
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

    // Arguments can be register/memory indexes or immediate values
    std::vector<int> arguments;

    Instruction(std::string, std::vector<int>);

    std::string __str__();
};

#endif
