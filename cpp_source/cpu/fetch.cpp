/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

#include "opcodes.h"

#include "simulation.h"
using namespace Simulation;

Fetch::Fetch(Cpu *cpu) : Pipeline("Fetch")
{
    this->cpu = cpu;
}

void Fetch::tick()
{
    RawInstruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction fetched\n";
        return;
    }

    std::cout << "Fetch processing instruction: " << instruction << "\n";

    // TODO move branch predicting logic into a branch prediction unit
    if (getOpcode(instruction->data) == 0b1100011)
    { // Branch detected, attempt a prediction

        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;
        this->cpu->programCounter.jump(getImmediateSB(instruction->data));

        std::cout << "Branch to " << this->cpu->programCounter << " predicted\n";
    }

    // Stop fetching if halt is encountered
    if (instruction->data != 0) // 0 is an invalid code in risc-v
    {
        this->next->stage(instruction);
        Event *fetch = new Event("Fetch", simulationClock.cycle + 1, this);
        masterEventQueue.push(fetch);
    }
    else
    {
        // Give downstream pipelines time to complete
        Event *complete = new Event("Complete", simulationClock.cycle + this->cpu->pipelines.size(), this->cpu);
        masterEventQueue.push(complete);
    }
}

void Fetch::process(Event *event)
{
    if (event->type == "Fetch")
    {
        event->handled = true;
        this->stage(new RawInstruction(this->cpu->ram.read<uint32_t>(this->cpu->programCounter.value)));
        ++this->cpu->programCounter;
    }

    Pipeline::process(event);
}
