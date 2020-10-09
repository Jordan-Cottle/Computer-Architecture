/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "arithmetic_instruction.h"

#include "cpu.h"

ArithmeticInstruction::ArithmeticInstruction(Instruction *instruction) : DecodedInstruction(instruction)
{
    this->immediate = false;

    this->destinationIndex = instruction->arguments[0];
    this->leftIndex = instruction->arguments[1];
    this->rightIndex = instruction->arguments[2];
}

ArithmeticInstruction::ArithmeticInstruction(Instruction *instruction, int immediateValue) : DecodedInstruction(instruction)
{
    this->immediate = true;

    this->destinationIndex = instruction->arguments[0];
    this->leftIndex = instruction->arguments[1];

    this->immediateValue = immediateValue;
}

Add::Add(Instruction *instruction) : ArithmeticInstruction(instruction)
{
}
Add::Add(Instruction *instruction, int immediateValue) : ArithmeticInstruction(instruction, immediateValue)
{
}

void Add::execute(Cpu *cpu)
{
    if (this->isFp)
    {
        double left = cpu->fpRegister.read(this->leftIndex);

        double right = cpu->fpRegister.read(this->rightIndex);

        cpu->fpRegister.write(this->destinationIndex, right + left);
    }
    else
    {
        int left = cpu->intRegister.read(this->leftIndex);

        int right = this->immediate ? this->immediateValue : cpu->intRegister.read(this->rightIndex);

        cpu->intRegister.write(this->destinationIndex, left + right);
    }
}

std::string Add::__str__()
{
    std::string prefix;
    if (this->operation[0] == 'f')
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
        s += "#" + str(this->immediateValue);
    }
    else
    {
        s += prefix + str(this->rightIndex);
    }

    return s + ")";
}
