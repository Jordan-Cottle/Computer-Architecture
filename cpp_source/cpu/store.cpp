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
    Pipeline::tick();

    if (this->free())
    {
        std::cout << "No instruction to store\n";
        return;
    }
    if (this->busy())
    {
        std::cout << "Store continuing to work on its task\n";
        return;
    }
    this->_busy = true;

    Store *instruction = (Store *)this->staged();

    std::cout << "Store processing instruction: " << instruction << "\n";

    instruction->execute(this->cpu);

    // No further reference to instruction will be created
    delete instruction;
    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
