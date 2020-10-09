/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "cpu.h"
#include "fetch.h"

#include "simulation.h"

using namespace Simulation;

#define REGISTER_COUNT 4
#define MEMORY_COUNT 1024
#define INSTRUCTION_MEMORY_COUNT 8

Cpu::Cpu() : SimulationDevice("Cpu"),
             intRegister(Register<int>(REGISTER_COUNT)),
             fpRegister(Register<double>(REGISTER_COUNT)),
             intMemory(Register<int>(MEMORY_COUNT)),
             fpMemory(Register<double>(MEMORY_COUNT))
{
    this->programCounter = 0;
    this->branchSpeculated = false;
    this->jumpedFrom = -1;

    this->pipelines = {};
    this->complete = false;
}

Cpu *Cpu::addPipeline(Pipeline *pipeline)
{
    if (!this->pipelines.empty())
    {
        this->pipelines.back()->next = pipeline;
    }

    this->pipelines.push_back(pipeline);

    return this;
}

void Cpu::tick()
{
    std::cout << simulationClock << "\n";
    for (auto pipeline : this->pipelines)
    {
        pipeline->tick();
    }
}

void Cpu::loadProgram(Program *program)
{
    this->program = program;
    this->programCounter = 0;
}

void Cpu::flush()
{
    for (auto pipeline : this->pipelines)
    {
        pipeline->flush();
        masterEventQueue.flush(simulationClock.cycle + 1, pipeline);

        // Don't flush anything past the execute stage
        if (pipeline->type == "Execute")
        {
            break;
        }
    }

    // Fetch event was deleted, put it back
    if (!this->complete)
    {
        Fetch *fetchUnit = dynamic_cast<Fetch *>(this->getPipeline("Fetch"));

        FetchEvent *fetchEvent = new FetchEvent(simulationClock.cycle + 1, fetchUnit);

        masterEventQueue.push(fetchEvent);
    }
}

Pipeline *Cpu::getPipeline(std::string type)
{
    for (auto pipeline : this->pipelines)
    {
        if (pipeline->type == type)
        {
            return pipeline;
        }
    }

    return NULL;
}

std::string Cpu::__str__()
{
    std::string s = "Cpu{\n";

    s += "\tRegisters: {\n";
    s += "\t\tInteger " + addIndent(str(this->intRegister), 2) + "\n";
    s += "\t\tFloat " + addIndent(str(this->fpRegister), 2) + "\n";

    s += "\t}\n\tMemory: {\n";
    s += "\t\tInteger " + addIndent(str(this->intMemory), 2) + "\n";
    s += "\t\tFloat " + addIndent(str(this->fpMemory), 2) + "\n";

    s += "\t}\n\tPipelines: {\n";
    for (auto pipeline : this->pipelines)
    {
        s += "\t\t" + addIndent(str(pipeline), 2) + "\n";
    }

    s += "\t}\n}";

    return s;
}
