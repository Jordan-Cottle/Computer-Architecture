/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __ARITHMETIC_INSTRUCTION__
#define __ARITHMETIC_INSTRUCTION__

#include "instruction.h"

struct ArithmeticInstruction : DecodedInstruction
{
    int destinationIndex;

    int leftIndex;
    int rightIndex;
    bool immediate;
    int immediateValue;

    ArithmeticInstruction(Instruction *instruction) : DecodedInstruction(instruction)
    {
        this->immediate = false;

        this->destinationIndex = instruction->arguments[0];
        this->leftIndex = instruction->arguments[1];
        this->rightIndex = instruction->arguments[2];
    }

    ArithmeticInstruction(Instruction *instruction, int immediateValue) : DecodedInstruction(instruction)
    {
        this->immediate = true;

        this->destinationIndex = instruction->arguments[0];
        this->leftIndex = instruction->arguments[1];

        this->immediateValue = immediateValue;
    }

    virtual void execute(Cpu *cpu) = 0;
};

struct Add : ArithmeticInstruction
{
    Add(Instruction *instruction) : ArithmeticInstruction(instruction)
    {
    }
    Add(Instruction *instruction, int immediateValue) : ArithmeticInstruction(instruction, immediateValue)
    {
    }

    void execute(Cpu *cpu)
    {
        if (this->isFp)
        {
            double left = cpu->fpRegister.read(this->leftIndex);

            double right = cpu->fpRegister.read(this->rightIndex);

            cpu->fpRegister.write(this->destinationIndex, right + left);
        }
        else
        {
            int left = cpu->fpRegister.read(this->leftIndex);

            int right = this->immediate ? this->immediateValue : cpu->intRegister.read(this->rightIndex);

            cpu->fpRegister.write(this->destinationIndex, right + left);
        }
    }

    std::string __str__()
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

        std::string s = prefix + str(this->destinationIndex) + " <- " + prefix + str(this->leftIndex) + " + ";
        if (this->immediate)
        {
            s += "#" + str(this->immediateValue);
        }
        else
        {
            s += prefix + str(this->rightIndex);
        }

        return s;
    }
};

#endif
