/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "cpu.h"

#include "fetch.h"
#include "decode.h"

#define REGISTER_COUNT 2
#define MEMORY_COUNT 4
#define INSTRUCTION_MEMORY_COUNT 8

Cpu::Cpu() : SimulationDevice("Cpu"),
             intRegister(Register<int>(REGISTER_COUNT)),
             fpRegister(Register<double>(REGISTER_COUNT)),
             instructionMemory(Register<Instruction *>(INSTRUCTION_MEMORY_COUNT)),
             intMemory(Register<int>(MEMORY_COUNT)),
             fpMemory(Register<double>(MEMORY_COUNT))
{
    this->programCounter = 0;

    this->pipelines = {};
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

void Cpu::tick(ulong time, EventQueue *eventQueue)
{
    for (auto pipeline : this->pipelines)
    {
        pipeline->tick(time, eventQueue);
    }

    this->programCounter += 1;
}

std::string Cpu::__str__()
{
    std::string s = "Cpu{\n";

    s += "\tRegisters: {\n";
    s += "\t\tInteger" + addIndent(str(this->intRegister), 2) + "\n";
    s += "\t\tFloat" + addIndent(str(this->fpRegister), 2) + "\n";

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