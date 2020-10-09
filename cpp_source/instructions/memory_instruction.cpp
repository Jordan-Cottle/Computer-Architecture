/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "memory_instruction.h"

#include "cpu.h"

MemoryInstruction::MemoryInstruction(Instruction *instruction, Register<int> *cpuRegister) : DecodedInstruction(instruction)
{
    this->registerIndex = arguments[0];
    this->memoryLocation = cpuRegister->read(arguments[1]);
}

Store::Store(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction(instruction, cpuRegister)
{
}

// Execute/Store
void Store::execute(Cpu *cpu)
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

std::string Store::__str__()
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

    return MemoryInstruction::__str__() + " (" + prefix + "M" + str(this->memoryLocation) + " <- " + prefix + str(this->registerIndex) + ")";
}

Load::Load(Instruction *instruction, Register<int> *cpuRegister) : MemoryInstruction(instruction, cpuRegister)
{
}

void Load::execute(Cpu *cpu)
{
    if (this->isFp)
    {
        double data = cpu->fpMemory.read(this->memoryLocation);

        cpu->fpRegister.write(this->registerIndex, data);
    }
    else
    {
        int data = cpu->intMemory.read(this->memoryLocation);

        cpu->intRegister.write(this->registerIndex, data);
    }
}

std::string Load::__str__()
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

    return MemoryInstruction::__str__() + " (" + prefix + str(this->registerIndex) + " <- " + prefix + "M" + str(this->memoryLocation) + ")";
}
