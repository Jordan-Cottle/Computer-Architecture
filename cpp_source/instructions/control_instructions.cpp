/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "control_instructions.h"
#include "cpu.h"

#include "opcodes.h"

BranchInstruction::BranchInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
    this->destination = getImmediateSB(instruction->data);
}

bool BranchInstruction::take(Cpu *cpu)
{
    return true; // Default branch behavior is to always take
}

void BranchInstruction::execute(Cpu *cpu)
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

    // Fetch unit has already made a correct prediction, nothing to do
}

std::string BranchInstruction::__str__()
{
    return DecodedInstruction::__str__() + " (PC -> " + str(this->destination) + ")";
}

Bne::Bne(RawInstruction *instruction) : BranchInstruction(instruction)
{
    this->leftIndex = getR1(instruction->data);
    this->rightIndex = getR2(instruction->data);
}

bool Bne::take(Cpu *cpu)
{
    return cpu->intRegister.read(this->leftIndex) != cpu->intRegister.read(this->rightIndex);
}

std::string Bne::__str__()
{
    return BranchInstruction::__str__() + " if R" + str(this->leftIndex) + " != R" + str(this->rightIndex);
}
