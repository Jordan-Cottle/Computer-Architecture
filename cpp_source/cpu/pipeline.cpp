/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include <iostream>

#include "pipeline.h"
#include "simulation.h"
using namespace Simulation;

PipelineInsertEvent::PipelineInsertEvent(ulong time, Pipeline *pipeline, Instruction *instruction) : Event("PipelineInsertEvent", time, pipeline), instruction(instruction)
{
}

std::string PipelineInsertEvent::__str__()
{
    std::string s = this->device->type + " " + Event::__str__();

    s += " <" + str(this->instruction) + ">";

    return s;
}

Pipeline::Pipeline(std::string type) : SimulationDevice(type),
                                       memory(Register<Instruction *>(1))
{
    this->next = NULL;
    this->type = type;
}

Pipeline::Pipeline(std::string type, Pipeline *next) : SimulationDevice(type),
                                                       memory(Register<Instruction *>(1))
{
    this->next = next;
    this->type = type;
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

void Pipeline::tick()
{
    this->flush();
    SimulationDevice::tick();
}

void Pipeline::process(Event *event)
{
    std::cout << this->type << " processing event: " << event << "\n";
    if (event->type == "PipelineInsertEvent")
    {
        event->handled = true;
        PipelineInsertEvent *insert = dynamic_cast<PipelineInsertEvent *>(event);
        Instruction *instruction = insert->instruction;

        this->stage(instruction);
    }

    SimulationDevice::process(event);
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
