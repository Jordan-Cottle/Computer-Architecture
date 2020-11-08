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
    if (this->next->busy())
    {
        std::cout << "Execute waiting because next stage is busy\n";
        return;
    }
    if (this->busy())
    {
        std::cout << "Execute continuing to work on its task\n";
        return;
    }
    this->_busy = true;

    DecodedInstruction *instruction = (DecodedInstruction *)this->staged();

    std::cout << "Execute processing instruction: " << instruction << "\n";

    Store *store = dynamic_cast<Store *>(instruction);
    Event *workCompleted;
    if (store != NULL)
    {
        this->next->stage(instruction);
        workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    }
    else
    {
        instruction->execute(this->cpu);

        // Decoded instruction use complete. No further reference to it will be created
        delete instruction;
        workCompleted = new Event("WorkCompleted", simulationClock.cycle + instruction->executionTime * 10, this, HIGH);
    }

    this->cpu->instructionsProcessed++;
    masterEventQueue.push(workCompleted);
}
