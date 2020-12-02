/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "arithmetic_instruction.h"

#include "cpu.h"
#include "binary.h"
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
        this->rightIndex = sign_extend(getImmediateI(instruction->data), 11);
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

        // std::cout << "F" + str(this->destinationIndex) << " <- " << str(left) << " + " << str(right) << "\n";
        cpu->fpRegister.write(this->destinationIndex, left + right);
    }
    else
    {
        int left = cpu->intRegister.read(this->leftIndex);

        int right = this->immediate ? this->rightIndex : cpu->intRegister.read(this->rightIndex);

        // std::cout << "x" + str(this->destinationIndex) << " <- " << str(left) << " + " << str(right) << "\n";
        cpu->intRegister.write(this->destinationIndex, left + right);
    }
}

std::string Add::__str__()
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

Sub::Sub(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
}

void Sub::execute(Cpu *cpu)
{
    if (this->isFp)
    {
        float left = cpu->fpRegister.read(this->leftIndex);

        float right = cpu->fpRegister.read(this->rightIndex);

        // std::cout << "F" + str(this->destinationIndex) << " <- " << str(left) << " - " << str(right) << "\n";
        cpu->fpRegister.write(this->destinationIndex, left - right);
    }
    else
    {
        int left = cpu->intRegister.read(this->leftIndex);

        int right = this->immediate ? this->rightIndex : cpu->intRegister.read(this->rightIndex);

        // std::cout << "x" + str(this->destinationIndex) << " <- " << str(left) << " - " << str(right) << "\n";
        cpu->intRegister.write(this->destinationIndex, left - right);
    }
}

std::string Sub::__str__()
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

    std::string s = DecodedInstruction::__str__() + " (" + prefix + str(this->destinationIndex) + " <- " + prefix + str(this->leftIndex) + " - ";
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
    // std::cout << "x" + str(this->destinationIndex) << " <- " << str(this->leftIndex) << "\n";
    cpu->intRegister.write(this->destinationIndex, this->leftIndex);
}

Slli::Slli(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
    // Shift can be at most 31 bits (32 bit shift would result in all 0s)
    this->rightIndex = this->rightIndex & 0x1F;
}

void Slli::execute(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    // std::cout << "x" + str(this->destinationIndex) << " <- " << str(left) << " << " << str(this->rightIndex) << "\n";
    cpu->intRegister.write(this->destinationIndex, left << this->rightIndex);
}

Multiply::Multiply(RawInstruction *instruction) : ArithmeticInstruction(instruction)
{
}

void Multiply::execute(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    int right = cpu->intRegister.read(this->rightIndex);

    int result = left * right;

    // std::cout << "x" + str(this->destinationIndex) << " <- " << str(left) << " * " << str(right) << "\n";

    cpu->intRegister.write(this->destinationIndex, result);
}
