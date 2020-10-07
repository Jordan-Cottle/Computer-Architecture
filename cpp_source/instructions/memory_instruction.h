/*
    Author: Jordan Cottle
    Created: 10/06/2020
*/

#ifndef __MEMORY_INSTRUCTION__
#define __MEMORY_INSTRUCTION__

#include "instruction.h"
#include "sim_register.h"

struct Cpu;
struct MemoryInstruction : DecodedInstruction
{

    int registerIndex;
    int memoryLocation;

    MemoryInstruction(Instruction *instruction, Register<int> *cpuRegister);

    virtual void execute(Cpu *cpu) = 0;
};

struct Store : MemoryInstruction
{
    Store(Instruction *instruction, Register<int> *cpuRegister);

    // Execute/Store
    void execute(Cpu *cpu);

    std::string __str__();
};

struct Load : MemoryInstruction
{
    Load(Instruction *instruction, Register<int> *cpuRegister);

    void execute(Cpu *cpu);

    std::string __str__();
};

#endif
