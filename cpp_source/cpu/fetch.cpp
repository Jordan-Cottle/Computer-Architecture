/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

FetchEvent::FetchEvent(ulong time, Fetch *device, int address) : Event("FetchEvent", time, device)
{
    this->address = address;
}

std::string FetchEvent::__str__()
{
    return "FetchEvent " + str(this->id) + " IM(" + str(this->address) + ")";
}

Fetch::Fetch(Pipeline *next, Register<Instruction *> *instructionMemory) : Pipeline("Fetch", next)
{
    this->source = instructionMemory;
}

void Fetch::tick(ulong time, EventQueue *eventQueue)
{
    if (this->staged() != NULL)
    {
        PipelineInsertEvent *event = new PipelineInsertEvent(time + 1, this->staged(), this->next);

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
        FetchEvent *fetchEvent = dynamic_cast<FetchEvent *>(event);
        this->stage(this->source->read(fetchEvent->address));
    }
    else if (event->type == "PipelineInsertEvent")
    {
        throw UnrecognizedEvent("Fetch units do not accept generic PipelineInsertEvents");
    }

    Pipeline::process(event, eventQueue);
}
