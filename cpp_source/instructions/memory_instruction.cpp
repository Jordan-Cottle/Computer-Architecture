/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "memory_instruction.h"

#include "cpu.h"

#include "binary.h"
#include "opcodes.h"

MemoryInstruction::MemoryInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->baseMemoryLocationRegisterIndex = getR1(instruction->data);

    this->width = 1 << ((instruction->data & 0x3000) >> 12);
}

uint32_t MemoryInstruction::memoryAddress(Cpu *cpu)
{
    return cpu->intRegister.read(this->baseMemoryLocationRegisterIndex) + this->memoryOffset;
}

Store::Store(RawInstruction *instruction) : MemoryInstruction(instruction)
{
    this->targetRegisterIndex = getR2(instruction->data);

    this->memoryOffset = getImmediateS(instruction->data);

    this->memoryOffset = sign_extend(this->memoryOffset, 11);
}

// Execute/Store
void Store::execute(Cpu *cpu)
{
    uint32_t memAddress = this->memoryAddress(cpu);

    if (this->isFp)
    {
        float data = cpu->fpRegister.read(this->targetRegisterIndex);

        OUT << "Storing: " << data << " into memory address " << str(memAddress) << "\n";
        cpu->memory->write(memAddress, MFMT(data));
    }
    else
    {
        if (this->targetRegisterIndex == 0)
        {
            throw std::runtime_error("Stores to register 0 have no effect!");
        }

        uint32_t data = cpu->intRegister.read(this->targetRegisterIndex);

        cpu->memory->write(memAddress, (void *)&data, this->width);
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

    // Handle 12 bit 2's compliment
    this->memoryOffset = sign_extend(this->memoryOffset, 11);

    this->signExtend = !getBit(instruction->data, 14);
}

void Load::execute(Cpu *cpu)
{
    uint32_t memAddress = this->memoryAddress(cpu);
    if (this->isFp)
    {
        float data = cpu->memory->readFloat(memAddress);

        cpu->fpRegister.write(this->targetRegisterIndex, data);
    }
    else
    {
        int data = cpu->memory->readInt(memAddress);

        uint32_t mask;
        switch (this->width)
        {
        case 1:
            mask = 0xFF;
            break;
        case 2:
            mask = 0xFFFF;
            break;
        case 4:
            mask = 0xFFFFFFFF;
            break;
        default:
            throw std::logic_error("Unrecognized load width: " + str(this->width));
        }

        data = data & mask;

        if (this->signExtend)
        {
            uint8_t index = (4 << this->width) - 1;
            data = sign_extend(data, index);
        }

        OUT << "Loading " << str(data) << " into integer register " << str(this->targetRegisterIndex) << "\n";
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
