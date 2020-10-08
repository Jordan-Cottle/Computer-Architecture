/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "branch_instructions.h"
#include "cpu.h"

DecodedBranch::DecodedBranch(Branch *branch, int destination) : DecodedInstruction(branch)
{
    this->destination = destination;
}

bool DecodedBranch::take(Cpu *cpu)
{
    return true; // Default branch behavior is to always take
}

void DecodedBranch::execute(Cpu *cpu)
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
            cpu->programCounter = cpu->jumpedFrom + 1;
        }
    }

    // Fetch unit has already made a correct prediction, nothing to do
}

std::string DecodedBranch::__str__()
{
    return "PC -> " + str(this->destination);
}
