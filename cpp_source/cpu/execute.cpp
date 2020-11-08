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
    DecodedInstruction *instruction = (DecodedInstruction *)this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to execute\n";
        return;
    }

    std::cout << "Execute processing instruction: " << instruction << "\n";
    Store *store = dynamic_cast<Store *>(instruction);
    if (store != NULL)
    {
        this->next->stage(instruction);
    }
    else
    {
        instruction->execute(this->cpu);

        // Decoded instruction use complete. No further reference to it will be created
        delete instruction;
    }

    this->cpu->instructionsProcessed++;
    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
