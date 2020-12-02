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

        // std::cout << "Storing: " << data << " into memory address " << str(memAddress) << "\n";
        cpu->memory->write(memAddress, data);
    }
    else
    {
        if (this->targetRegisterIndex == 0)
        {
            throw std::runtime_error("Stores to register 0 have no effect!");
        }

        int data = cpu->intRegister.read(this->targetRegisterIndex);

        uint8_t offset = 32 - (this->width * 8);
        uint32_t srcMask;
        uint32_t originMask;
        switch (this->width)
        {
        case 1:
            srcMask = 0x000000FF;
            originMask = 0x00FFFFFF;
            break;
        case 2:
            srcMask = 0x0000FFFF;
            originMask = 0x0000FFFF;
            break;
        case 4:
            srcMask = 0xFFFFFFFF;
            originMask = 0x00000000;
            break;
        default:
            throw std::logic_error("Unrecognized store width: " + str(this->width));
        }

        data = data & srcMask;
        // std::cout << "Storing: " << str(data) << " into memory address " << str(memAddress) << "\n";

        // Overlay previous data so we can use full 32 bit write method
        // all this bit mangling wouldn't be necessary if templates were overridable in sub classes
        // alternative approach would be to add 4 new methods to the MemoryInterface for
        // Write half, write byte, read half, read byte
        // 10-20 lines of bit mangling in the memory instructions was better than
        // 4 new methods that need to be added to 3+ different classes.
        // Could maybe also change up the write method to accept a void pointer and a length
        // And remove the duplicated writes for different types altogether
        data = data << offset; // move data into upper bits
        uint32_t existingData = cpu->memory->readUint(memAddress) & originMask;

        data = data | existingData;

        // std::cout << "Writing: " << str(data) << " into memory address " << str(memAddress) << "\n";
        cpu->memory->write(memAddress, data);
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
        uint8_t offset = 32 - (this->width * 8);
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

        data = (data >> offset) & mask;

        if (this->signExtend)
        {
            uint8_t index = (4 << this->width) - 1;
            data = sign_extend(data, index);
        }

        // std::cout << "Loading " << str(data) << " into integer register " << str(this->targetRegisterIndex) << "\n";
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
