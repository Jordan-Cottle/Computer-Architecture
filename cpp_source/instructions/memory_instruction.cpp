/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "memory_instruction.h"

#include "cpu.h"
#include "opcodes.h"

MemoryInstruction::MemoryInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->baseMemoryLocationRegisterIndex = getR1(instruction->data);
}

Store::Store(RawInstruction *instruction) : MemoryInstruction(instruction)
{
    this->targetRegisterIndex = getR2(instruction->data);

    int offset = getImmediateS(instruction->data);

    // Convert 12th bit to negative
    offset += -4096 * ((int)getBit(offset, 11) >> 11);

    this->memoryOffset = offset;
}

// Execute/Store
void Store::execute(Cpu *cpu)
{
    uint32_t memAddress = cpu->intRegister.read(this->baseMemoryLocationRegisterIndex) + this->memoryOffset;

    if (this->isFp)
    {
        float data = cpu->fpRegister.read(this->targetRegisterIndex);

        std::cout << "Storing: " << data << " into memory address " << str(memAddress) << "\n";
        cpu->ram.write(memAddress, data);
    }
    else
    {
        int data = cpu->intRegister.read(this->targetRegisterIndex);

        std::cout << "Storing: " << data << " into memory address " << str(memAddress) << "\n";
        cpu->ram.write(memAddress, data);
    }
}

std::string Store::__str__()
{
    std::string prefix;
    if (this->isFp)
    {
        prefix = "F";
    }
    else
    {
        prefix = "R";
    }

    return MemoryInstruction::__str__() + " " + str(this->memoryOffset) + "(x" + str(this->baseMemoryLocationRegisterIndex) + ") <- " + prefix + str(this->targetRegisterIndex);
}

Load::Load(RawInstruction *instruction) : MemoryInstruction(instruction)
{
    this->targetRegisterIndex = getRd(instruction->data);
    this->memoryOffset = getImmediateI(instruction->data);
}

void Load::execute(Cpu *cpu)
{
    uint32_t memAddress = cpu->intRegister.read(this->baseMemoryLocationRegisterIndex) + this->memoryOffset;
    if (this->isFp)
    {
        float data = cpu->ram.read<float>(memAddress);

        cpu->fpRegister.write(this->targetRegisterIndex, data);
    }
    else
    {
        int data = cpu->ram.read<int>(memAddress);

        cpu->intRegister.write(this->targetRegisterIndex, data);
    }
}

std::string Load::__str__()
{
    std::string prefix;
    if (this->isFp)
    {
        prefix = "F";
    }
    else
    {
        prefix = "R";
    }

    return MemoryInstruction::__str__() + " " + prefix + str(this->targetRegisterIndex) + " <- " + str(this->memoryOffset) + "(x" + str(this->baseMemoryLocationRegisterIndex) + ")";
}
