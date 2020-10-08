/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

FetchEvent::FetchEvent(ulong time, Fetch *device) : Event("FetchEvent", time, device)
{
}

std::string FetchEvent::__str__()
{
    return "FetchEvent " + str(this->id);
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
    }
    else
    {
        PipelineInsertEvent *event = new PipelineInsertEvent(time + 1, this->next, instruction);

        eventQueue->push(event);
    }

    FetchEvent *fetch = new FetchEvent(time + 1, this);
    eventQueue->push(fetch);
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
