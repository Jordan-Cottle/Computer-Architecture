/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "store.h"

#include "memory_instruction.h"

#include "cpu.h"

#include "simulation.h"
using namespace Simulation;

StorePipeline::StorePipeline(Cpu *cpu) : Pipeline("StorePipeline")
{
    this->cpu = cpu;
}

void StorePipeline::tick()
{
    Store *instruction = (Store *)this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to store\n";
        return;
    }

    std::cout << "Store processing instruction: " << instruction << "\n";

    instruction->execute(this->cpu);

    // No further reference to instruction will be created
    delete instruction;
    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
