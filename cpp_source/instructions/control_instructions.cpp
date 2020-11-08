/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "control_instructions.h"
#include "cpu.h"

#include "opcodes.h"

ControlInstruction::ControlInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->destination = getImmediateSB(instruction->data);
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
            cpu->programCounter.jump(this->destination);
        }
        else
        {
            cpu->programCounter.jump(cpu->jumpedFrom);
        }
    }

    if (take)
    {
        std::cout << "Jumping to " << str(this->destination) << "\n";
    }

    // Fetch unit has already made a correct prediction, nothing to do
    // TODO actual PC should be tracked separate from fetch unit's predictions
}

std::string ControlInstruction::__str__()
{
    return DecodedInstruction::__str__() + " (PC -> " + str(this->destination) + ")";
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
    std::cout << "Branching to " << str(this->destination) << " if " << str(left) << " != " << str(right) << "\n";
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
    std::cout << "Branching to " << str(this->destination) << " if " << str(left) << " < " << str(right) << "\n";
    return left < right;
}

std::string Blt::__str__()
{
    return ControlInstruction::__str__() + " if R" + str(this->leftIndex) + " < R" + str(this->rightIndex);
}

Jump::Jump(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->destination = getImmediateUB(instruction->data);
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
    this->destination = twos_compliment(getImmediateI(instruction->data), 12);
}

void Jalr::execute(Cpu *cpu)
{
    this->destination += cpu->intRegister.read(this->sourceIndex);

    cpu->intRegister.write(this->registerIndex, cpu->jumpedFrom);

    ControlInstruction::execute(cpu);
}
