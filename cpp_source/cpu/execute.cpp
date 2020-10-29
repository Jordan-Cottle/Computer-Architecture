/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "execute.h"

#include "memory_instruction.h"

#include "simulation.h"
using namespace Simulation;

Execute::Execute(Cpu *cpu) : Pipeline("Execute")
{
    this->cpu = cpu;
}

void Execute::tick()
{
    Instruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to execute\n";
        return;
    }

    std::cout << "Execute processing instruction: " << instruction << "\n";
    Store *store = dynamic_cast<Store *>(instruction);
    if (store != NULL || instruction->operation == "halt")
    {
        this->next->stage(instruction);
    }
    else
    {
        DecodedInstruction *decoded = dynamic_cast<DecodedInstruction *>(instruction);

        decoded->execute(this->cpu);

        // Decoded instructions are not pointers to the ones in the program
        delete decoded;
    }
}
