/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

FetchEvent::FetchEvent(int time, Fetch *device, int address) : Event("FetchEvent", time, device)
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

void Fetch::tick()
{
    this->flush();
}

void Fetch::process(Event *event, EventQueue *eventQueue)
{
    if (event->type == "FetchEvent")
    {
        FetchEvent *fetchEvent = dynamic_cast<FetchEvent *>(event);
        this->stage(this->source->read(fetchEvent->address));

        // TODO: Set stage event for next pipeline
    }
    else if (event->type == "PipelineInsertEvent")
    {
        throw UnrecognizedEvent("Fetch units do not accept generic PipelineInsertEvents");
    }
    else
    {
        Pipeline::process(event, eventQueue);
    }
}
