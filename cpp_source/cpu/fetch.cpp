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
    if (this->staged() != NULL)
    {
        PipelineInsertEvent *event = new PipelineInsertEvent(time + 1, this->next, this->staged());

        eventQueue->push(event);
    }
    else
    {
        std::cout << "No instructions fetched\n";
    }

    Pipeline::tick(time, eventQueue);
}

void Fetch::process(Event *event, EventQueue *eventQueue)
{
    if (event->type == "FetchEvent")
    {
        event->handled = true;
        this->stage(this->cpu->instructionMemory.read(this->cpu->programCounter));
    }
    else if (event->type == "PipelineInsertEvent")
    {
        throw UnrecognizedEvent("Fetch units do not accept PipelineInsertEvents");
    }

    Pipeline::process(event, eventQueue);
}
