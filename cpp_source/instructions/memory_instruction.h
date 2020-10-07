/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __MEMORY_INSTRUCTION__
#define __MEMORY_INSTRUCTION__

#include "instruction.h"
#include "sim_register.h"
#include "cpu.h"

struct MemoryInstruction : DecodedInstruction
{

    int registerIndex;
    int memoryLocation;

    MemoryInstruction(Instruction *instruction, Register<int> *cpuRegister) : DecodedInstruction(instruction)
    {
        this->registerIndex = arguments[0];
        this->memoryLocation = cpuRegister->read(arguments[1]);
    }

    virtual void execute(Cpu *cpu) = 0;
};

struct Store : MemoryInstruction
{
    Store(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction(instruction, cpuRegister)
    {
    }

    // Execute/Store
    void execute(Cpu *cpu)
    {
        if (this->isFp)
        {
            double data = cpu->fpRegister.read(this->registerIndex);

            std::cout << "Storing: " << data << "\n";
            cpu->fpMemory.write(this->memoryLocation, data);
        }
        else
        {
            int data = cpu->intRegister.read(this->registerIndex);

            std::cout << "Storing: " << data << "\n";
            cpu->intMemory.write(this->memoryLocation, data);
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

        return prefix + "M" + str(this->memoryLocation) + " <- " + prefix + str(this->registerIndex);
    }
};

struct Load : MemoryInstruction
{
    Load(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction(instruction, cpuRegister)
    {
    }

    void execute(Cpu *cpu)
    {
        if (this->isFp)
        {
            double data = cpu->fpMemory.read(this->memoryLocation);

            std::cout << "Loading: " << data << "\n";

            cpu->fpRegister.write(this->registerIndex, data);
        }
        else
        {
            int data = cpu->intMemory.read(this->memoryLocation);

            std::cout << "Loading: " << data << "\n";

            cpu->intRegister.write(this->registerIndex, data);
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

        return prefix + str(this->registerIndex) + " <- " + prefix + "M" + str(this->memoryLocation);
    }
};

#endif
