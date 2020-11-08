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
    Pipeline::tick();

    if (this->free())
    {
        std::cout << "No instruction to execute\n";
        return;
    }
    if (this->next->busy)
    {
        std::cout << "Execute waiting because next stage is busy\n";
        this->busy = true;
        return;
    }

    DecodedInstruction *instruction = (DecodedInstruction *)this->staged();

    std::cout << "Execute processing instruction: " << instruction << "\n";
    Store *store = dynamic_cast<Store *>(instruction);
    if (store != NULL)
    {
        this->next->stage(instruction);
    }
    else
    {
        this->busy = true;
        instruction->execute(this->cpu);

        // Decoded instruction use complete. No further reference to it will be created
        delete instruction;
    }

    this->cpu->instructionsProcessed++;
    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
