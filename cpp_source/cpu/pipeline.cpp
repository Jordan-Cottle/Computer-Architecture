/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include <iostream>

#include "pipeline.h"

PipelineInsertEvent::PipelineInsertEvent(int time, Instruction *instruction, Pipeline *pipeline) : Event("PipelineInsertEvent", time, pipeline), instruction(instruction)
{
}

PipelineFlushEvent::PipelineFlushEvent(int time, Pipeline *pipeline) : Event("PipelineFlushEvent", time, pipeline)
{
}

Pipeline::Pipeline(std::string type) : SimulationDevice(type),
                                       memory(Register<Instruction *>(1))
{
    this->next = NULL;
}

Pipeline::Pipeline(std::string type, Pipeline *next) : SimulationDevice(type),
                                                       memory(Register<Instruction *>(1))
{
    this->next = next;
}

bool Pipeline::free()
{
    return this->memory.read(0) == NULL;
}

void Pipeline::stage(Instruction *instruction)
{
    if (!this->free())
    {
        std::cout << "WARNING: " << this << "is not free!!\n";
        return;
    }

    this->memory.write(0, instruction);
}

void Pipeline::flush()
{
    this->memory.clear(0);
}

Instruction *Pipeline::staged()
{
    return this->memory.read(0);
}

void Pipeline::tick(EventQueue *eventQueue)
{
    this->flush();
    SimulationDevice::tick(eventQueue);
}

void Pipeline::process(Event *event, EventQueue *eventQueue)
{
    this->eventsProcessed += 1;

    std::cout << this->type << " processing " << event->type << "\n";
    if (event->type == "PipelineInsertEvent")
    {
        event->handled = true;
        PipelineInsertEvent *insert = dynamic_cast<PipelineInsertEvent *>(event);
        Instruction *instruction = insert->instruction;

        this->stage(instruction);
    }
    else if (event->type == "PipelineFlushEvent")
    {
        event->handled = true;
        this->flush();
    }
    else if (!event->handled)
    {
        throw UnrecognizedEvent(event->type);
    }

    SimulationDevice::process(event, eventQueue);
}

std::string Pipeline::__str__()
{
    std::string s = this->type + ":\n\t" + addIndent(str(this->memory));

    if (this->next != NULL)
    {
        s += "\n\tNext: " + this->next->type;
    }

    return s;
}