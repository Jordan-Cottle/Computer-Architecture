
/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include "instruction.h"

#include "opcodes.h"

RawInstruction::RawInstruction(uint32_t data)
{
    this->data = data;
}

std::string RawInstruction::keyword()
{
    return identify(this->data);
}

uint32_t RawInstruction::opcode()
{
    return this->data & O_MASK;
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

std::string DecodedInstruction::__str__()
{
    return this->keyword() + "\t" + DecodedInstruction::__str__();
}

DecodedInstruction::DecodedInstruction(RawInstruction *instruction) : RawInstruction(instruction->data)
{
    this->isFp = instruction->keyword()[0] == 'f';
}
