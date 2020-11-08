/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include <iostream>

#include "pipeline.h"
#include "simulation.h"
using namespace Simulation;

Pipeline::Pipeline(std::string type) : SimulationDevice(type),
                                       memory(Register<RawInstruction *>(1))
{
    this->next = NULL;
}

Pipeline::Pipeline(std::string type, Pipeline *next) : SimulationDevice(type),
                                                       memory(Register<RawInstruction *>(1))
{
    this->next = next;
}

bool Pipeline::free()
{
    return this->memory.read(0) == NULL;
}

void Pipeline::stage(RawInstruction *instruction)
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

RawInstruction *Pipeline::staged()
{
    return this->memory.read(0);
}

void Pipeline::process(Event *event)
{
    if (event->type == "WorkCompleted")
    {
        event->handled = true;
        this->flush();
        this->busy = false;
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
