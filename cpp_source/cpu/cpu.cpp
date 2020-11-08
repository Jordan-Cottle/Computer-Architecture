/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include <fstream>

#include "cpu.h"
#include "fetch.h"

#include "simulation.h"

using namespace Simulation;

constexpr int REGISTER_COUNT = 32;
constexpr int MEMORY_SIZE = 0x1400;
constexpr int MEMORY_DELAY = 20;
constexpr int SIM_CYCLES_PER_CPU = 10;
constexpr int MEMORY_ADDRESSES_PER_INSTRUCTION = 4;

Cpu::Cpu() : SimulationDevice("Cpu"),
             intRegister(Register<int>(REGISTER_COUNT, "Integer")),
             fpRegister(Register<float>(REGISTER_COUNT, "Float")),
             ram(Memory(MEMORY_SIZE, MEMORY_DELAY))
{
    this->programCounter = ProgramCounter(MEMORY_ADDRESSES_PER_INSTRUCTION);
    this->branchSpeculated = false;
    this->jumpedFrom = -1;

    this->pipelines = {};
    this->complete = false;
    this->instructionsProcessed = 0;
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

void Cpu::process(Event *event)
{
    if (event->type == "Complete")
    {
        event->handled = true;
        this->complete = true;
    }

    SimulationDevice::process(event);
}

void Cpu::tick()
{
    std::cout << "Cpu cycle " << this->clocksProcessed << ": " << this->programCounter << "\n";
    // Work pipelines backwards
    // This allows allows each stage to set the instruction into the next stage with worrying
    // about the instruction being boosted all the way through the pipeline in a single cycle
    int i = SIM_CYCLES_PER_CPU;
    for (auto pipeline : this->pipelines)
    {
        masterEventQueue.push(new Event("Tick", simulationClock.cycle + --i, pipeline));
    }

    masterEventQueue.push(new Event("Fetch", simulationClock.cycle + i, this->pipelines[0], 11));
    masterEventQueue.push(new Event("Tick", simulationClock.cycle + SIM_CYCLES_PER_CPU, this, 0));

    SimulationDevice::tick();
}

void Cpu::loadProgram(std::string fileName)
{
    std::ifstream programFile(fileName, std::ios::binary);

    if (!programFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint32_t instruction = 0;

    int memAddress = 0;
    while (programFile.read((char *)&instruction, sizeof(instruction)))
    {
        this->ram.write(memAddress, instruction);
        memAddress += sizeof(instruction);
    }
}

void Cpu::flush()
{
    for (auto pipeline : this->pipelines)
    {
        pipeline->flush();
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

float Cpu::cpi()
{
    return (float)this->clocksProcessed / this->instructionsProcessed;
}

std::string Cpu::__str__()
{
    std::string s = "Cpu{\n";

    s += "\t" + str(this->programCounter) + "\n";

    s += "\tRegisters: {\n";
    s += "\t\tInteger " + addIndent(str(this->intRegister), 2) + "\n";
    s += "\t\tFloat " + addIndent(str(this->fpRegister), 2) + "\n";

    s += "\t}\n\t" + addIndent(str(this->ram));

    s += "\t}\n\tPipelines: {\n";
    for (auto pipeline : this->pipelines)
    {
        s += "\t\t" + addIndent(str(pipeline), 2) + "\n";
    }

    s += "\t}\n}";

    return s;
}
