/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

#include "simulation.h"
using namespace Simulation;

Fetch::Fetch(Cpu *cpu) : Pipeline("Fetch")
{
    this->cpu = cpu;
}

void Fetch::tick()
{
    Instruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction fetched\n";
        return;
    }
    std::cout << "Fetch processing instruction: " << instruction << "\n";

    Branch *branch = dynamic_cast<Branch *>(instruction);

    if (branch != NULL)
    {
        this->cpu->branchSpeculated = true; // Predict True for all branches
        this->cpu->jumpedFrom = this->cpu->programCounter;
        this->cpu->programCounter = this->cpu->program->index(branch->label);

        std::cout << "Branch to " << this->cpu->programCounter << " predicted\n";
    }

    if (instruction->operation != "stall") // Don't pass on stall instructions
    {
        this->next->stage(instruction);
    }

    // Stop fetching if halt is encountered
    if (instruction->operation != "halt")
    {
        Event *fetch = new Event("Fetch", simulationClock.cycle + 1, this);
        masterEventQueue.push(fetch);
    }
}

void Fetch::process(Event *event)
{
    if (event->type == "Fetch")
    {
        event->handled = true;
        this->stage(this->cpu->program->line(this->cpu->programCounter++));
    }

    Pipeline::process(event);
}
