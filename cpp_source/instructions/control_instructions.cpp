/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "control_instructions.h"
#include "cpu.h"

BranchInstruction::BranchInstruction(Branch *branch, int destination) : DecodedInstruction(branch)
{
    this->destination = destination;
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
            cpu->programCounter = this->destination;
        }
        else
        {
            cpu->programCounter = cpu->jumpedFrom;
        }
    }

    // Fetch unit has already made a correct prediction, nothing to do
}

std::string BranchInstruction::__str__()
{
    return "PC -> " + str(this->destination);
}

Bne::Bne(Branch *branch, int destination) : BranchInstruction(branch, destination)
{
    this->leftIndex = branch->arguments[0];
    this->rightIndex = branch->arguments[1];
}

bool Bne::take(Cpu *cpu)
{
    return cpu->intRegister.read(this->leftIndex) != cpu->intRegister.read(this->rightIndex);
}
