/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __ARITHMETIC_INSTRUCTION__
#define __ARITHMETIC_INSTRUCTION__

#include "instruction.h"

template <typename T>
struct ArithmeticInstruction : Instruction
{
    int destinationIndex;

    int leftIndex;
    int rightIndex;
    bool immediate;
    T immediateValue;

    ArithmeticInstruction(Instruction *instruction) : Instruction(instruction->operation, instruction->arguments)
    {
        this->immediate = false;

        this->destinationIndex = instruction->arguments[0];
        this->leftIndex = instruction->arguments[1];
        this->rightIndex = instruction->arguments[2];
    }

    ArithmeticInstruction(Instruction *instruction, T immediateValue) : Instruction(instruction->operation, instruction->arguments)
    {
        this->immediate = true;

        this->destinationIndex = instruction->arguments[0];
        this->leftIndex = instruction->arguments[1];

        this->immediateValue = immediateValue;
    }

    virtual void execute(Register<T> *cpuRegister) = 0;
};

template <typename T>
struct Add : ArithmeticInstruction<T>
{
    Add(Instruction *instruction) : ArithmeticInstruction<T>(instruction)
    {
    }
    Add(Instruction *instruction, T immediateValue) : ArithmeticInstruction<T>(instruction, immediateValue)
    {
    }

    void execute(Register<T> *cpuRegister)
    {
        T left = cpuRegister->read(this->leftIndex);

        T right;
        if (this->immediate)
        {
            right = this->immediateValue;
        }
        else
        {
            right = cpuRegister->read(this->rightIndex);
        }

        cpuRegister->write(this->destinationIndex, right + left);
    }
};

#endif
