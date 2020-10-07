/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __MEMORY_INSTRUCTION__
#define __MEMORY_INSTRUCTION__

#include "instruction.h"
#include "sim_register.h"

template <typename T>
struct MemoryInstruction : Instruction
{

    int registerIndex;
    int memoryLocation;

    MemoryInstruction(Instruction *instruction, Register<int> *cpuRegister) : Instruction(instruction->operation, instruction->arguments)
    {
        this->registerIndex = arguments[0];
        this->memoryLocation = cpuRegister->read(arguments[1]);
    }

    virtual void execute(Register<T> *cpuRegister, Register<T> *memory) = 0;
};

template <typename T>
struct Store : MemoryInstruction<T>
{
    Store(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction<T>(instruction, cpuRegister)
    {
    }

    // Execute/Store
    void execute(Register<T> *cpuRegister, Register<T> *memory)
    {
        double data = cpuRegister->read(this->registerIndex);

        std::cout << "Storing: " << data << "\n";
        memory->write(this->memoryLocation, data);
    }
};

template <typename T>
struct Load : MemoryInstruction<T>
{
    Load(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction<T>(instruction, cpuRegister)
    {
    }

    void execute(Register<T> *cpuRegister, Register<T> *memory)
    {
        double data = memory->read(this->memoryLocation);

        std::cout << "Loading: " << data << "\n";

        cpuRegister->write(this->registerIndex, data);
    }
};

#endif
