/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

FetchEvent::FetchEvent(ulong time, Fetch *device) : Event("FetchEvent", time, device)
{
}

Fetch::Fetch(Cpu *cpu) : Pipeline("Fetch")
{
    this->cpu = cpu;
}

void Fetch::tick(ulong time, EventQueue *eventQueue)
{
    Instruction *instruction = this->staged();
    Pipeline::tick(time, eventQueue);

    Branch *branch = dynamic_cast<Branch *>(instruction);

    if (branch != NULL)
    {
        this->cpu->branchSpeculated = true; // Predict True for all branches
        this->cpu->jumpedFrom = this->cpu->programCounter;
        this->cpu->programCounter = this->cpu->program->index(branch->label);

        std::cout << "Branch to " << this->cpu->programCounter << " predicted\n";
    }

    if (instruction == NULL)
    {
        std::cout << "No instructions fetched\n";
        return;
    }
    else if (instruction->operation != "stall") // Don't pass on stall instructions
    {
        PipelineInsertEvent *event = new PipelineInsertEvent(time + 1, this->next, instruction);

        eventQueue->push(event);
    }

    // Stop fetching if halt is encountered
    if (instruction->operation != "halt")
    {
        FetchEvent *fetch = new FetchEvent(time + 1, this);
        eventQueue->push(fetch);
    }
}

void Fetch::process(Event *event, EventQueue *eventQueue)
{
    if (event->type == "FetchEvent")
    {
        event->handled = true;
        this->stage(this->cpu->program->line(this->cpu->programCounter++));
    }
    else if (event->type == "PipelineInsertEvent")
    {
        throw UnrecognizedEvent("Fetch units do not accept PipelineInsertEvents");
    }

    Pipeline::process(event, eventQueue);
}
