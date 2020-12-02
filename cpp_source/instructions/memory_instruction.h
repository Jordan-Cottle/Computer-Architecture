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

    int targetRegisterIndex;
    int baseMemoryLocationRegisterIndex;
    int memoryOffset;

    uint8_t width;

    MemoryInstruction(RawInstruction *instruction);

    uint32_t memoryAddress(Cpu *cpu);
    virtual void execute(Cpu *cpu) = 0;
};

struct Store : MemoryInstruction
{
    Store(RawInstruction *instruction);

    // Execute/Store
    void execute(Cpu *cpu);

    std::string __str__();
};

struct Load : MemoryInstruction
{
    bool signExtend;
    Load(RawInstruction *instruction);

    void execute(Cpu *cpu);

    std::string __str__();
};

#endif
