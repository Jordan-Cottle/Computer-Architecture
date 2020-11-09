/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "control_instructions.h"
#include "cpu.h"

#include "opcodes.h"

ControlInstruction::ControlInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->offset = sign_extend(getImmediateSB(instruction->data), 12);
}

bool ControlInstruction::take(Cpu *cpu)
{
    return true; // Default branch behavior is to always take
}

void ControlInstruction::execute(Cpu *cpu)
{
    bool take = this->take(cpu);
    if (this->take(cpu) != cpu->branchSpeculated)
    {
        std::cout << "Incorrect branch prediction!\n";
        cpu->flush();

        if (take)
        {
            cpu->programCounter.jump(this->offset);
        }
        else
        {
            cpu->programCounter.value = cpu->jumpedFrom;
        }
    }

    if (take)
    {
        std::cout << "Jumping by " << str(this->offset) << "\n";
    }

    // Fetch unit has already made a correct prediction, nothing to do
    // TODO actual PC should be tracked separate from fetch unit's predictions
}

std::string ControlInstruction::__str__()
{
    return DecodedInstruction::__str__() + " (PC += " + str(this->offset) + ")";
}

BranchInstruction::BranchInstruction(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->leftIndex = getR1(instruction->data);
    this->rightIndex = getR2(instruction->data);
}

Bne::Bne(RawInstruction *instruction) : BranchInstruction(instruction)
{
}

bool Bne::take(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    int right = cpu->intRegister.read(this->rightIndex);
    std::cout << "Jumping by " << str(this->offset) << " if " << str(left) << " != " << str(right) << "\n";
    return left != right;
}

std::string Bne::__str__()
{
    return ControlInstruction::__str__() + " if R" + str(this->leftIndex) + " != R" + str(this->rightIndex);
}

Blt::Blt(RawInstruction *instruction) : BranchInstruction(instruction)
{
}

bool Blt::take(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    int right = cpu->intRegister.read(this->rightIndex);
    std::cout << "Jumping by " << str(this->offset) << " if " << str(left) << " < " << str(right) << "\n";
    return left < right;
}

std::string Blt::__str__()
{
    return ControlInstruction::__str__() + " if R" + str(this->leftIndex) + " < R" + str(this->rightIndex);
}

Jump::Jump(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->offset = sign_extend(getImmediateUB(instruction->data), 20);
    this->registerIndex = getRd(instruction->data);
}

void Jump::execute(Cpu *cpu)
{
    cpu->intRegister.write(this->registerIndex, cpu->jumpedFrom);

    ControlInstruction::execute(cpu);
}

Jalr::Jalr(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->registerIndex = getRd(instruction->data);
    this->sourceIndex = getR1(instruction->data);
    this->offset = twos_compliment(getImmediateI(instruction->data), 12);
}

void Jalr::execute(Cpu *cpu)
{
    this->offset += cpu->intRegister.read(this->sourceIndex);

    if (this->offset == 0)
    {
        // Is this how we're supposed to end the program??
        std::cout << "Return to PC 0 detected, program complete\n";
        cpu->complete = true;
        return;
    }

    cpu->intRegister.write(this->registerIndex, cpu->jumpedFrom);

    ControlInstruction::execute(cpu);
}
