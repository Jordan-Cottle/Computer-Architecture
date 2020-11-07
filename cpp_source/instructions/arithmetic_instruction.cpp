/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "arithmetic_instruction.h"

#include "cpu.h"
#include "opcodes.h"

ArithmeticInstruction::ArithmeticInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->immediate = !this->isFp && getOpcode(instruction->data) == 0b0010011;

    this->destinationIndex = getRd(instruction->data);
    this->leftIndex = getR1(instruction->data);

    if (!this->immediate)
    {
        this->rightIndex = getR2(instruction->data);
    }
    else
    {
        this->rightIndex = getImmediateI(instruction->data);

        // Handle 12bit 2's compliment
        this->rightIndex = twos_compliment(this->rightIndex, 11);
    }
}

Add::Add(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
}

void Add::execute(Cpu *cpu)
{
    if (this->isFp)
    {
        float left = cpu->fpRegister.read(this->leftIndex);

        float right = cpu->fpRegister.read(this->rightIndex);

        cpu->fpRegister.write(this->destinationIndex, right + left);
    }
    else
    {
        int left = cpu->intRegister.read(this->leftIndex);

        int right = this->immediate ? this->rightIndex : cpu->intRegister.read(this->rightIndex);

        cpu->intRegister.write(this->destinationIndex, left + right);
    }
}

std::string Add::__str__()
{
    std::string prefix;
    if (this->immediate)
    {
        prefix = "F";
    }
    else
    {
        prefix = "R";
    }

    std::string s = DecodedInstruction::__str__() + " (" + prefix + str(this->destinationIndex) + " <- " + prefix + str(this->leftIndex) + " + ";
    if (this->immediate)
    {
        s += "#" + str(this->rightIndex);
    }
    else
    {
        s += prefix + str(this->rightIndex);
    }

    return s + ")";
}

Lui::Lui(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
    this->leftIndex = getImmediateU(instruction->data);
}

void Lui::execute(Cpu *cpu)
{
    cpu->intRegister.write(this->destinationIndex, this->leftIndex);
}

Slli::Slli(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
    // Shift can be at most 31 bits (32 bit shift would result in all 0s)
    this->rightIndex = this->rightIndex & 0x1F;
}

void Slli::execute(Cpu *cpu)
{
    cpu->intRegister.write(this->destinationIndex, this->leftIndex << this->rightIndex);
}
