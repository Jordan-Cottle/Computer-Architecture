/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "arithmetic_instruction.h"

#include "cpu.h"
#include "opcodes.h"

ArithmeticInstruction::ArithmeticInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->immediate = (instruction->data & (1 << 5)) == 0;

    this->destinationIndex = (instruction->data & RD_MASK) >> 7;
    this->leftIndex = (instruction->data & R1_MASK) >> 15;

    if (!this->immediate)
    {
        this->rightIndex = (instruction->data & R2_MASK) >> 20;
    }
    else
    {
        this->rightIndex == (instruction->data & IMM_I_MASK) >> 20;
    }
}

Add::Add(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
}

void Add::execute(Cpu *cpu)
{
    if (this->isFp)
    {
        float left = cpu->fpRegister.read(this->leftIndex);

        float right = cpu->fpRegister.read(this->rightIndex);

        cpu->fpRegister.write(this->destinationIndex, right + left);
    }
    else
    {
        int left = cpu->intRegister.read(this->leftIndex);

        int right = this->immediate ? this->rightIndex : cpu->intRegister.read(this->rightIndex);

        cpu->intRegister.write(this->destinationIndex, left + right);
    }
}

std::string Add::__str__()
{
    std::string prefix;
    if (this->immediate)
    {
        prefix = "F";
    }
    else
    {
        prefix = "R";
    }

    std::string s = DecodedInstruction::__str__() + " (" + prefix + str(this->destinationIndex) + " <- " + prefix + str(this->leftIndex) + " + ";
    if (this->immediate)
    {
        s += "#" + str(this->rightIndex);
    }
    else
    {
        s += prefix + str(this->rightIndex);
    }

    return s + ")";
}
