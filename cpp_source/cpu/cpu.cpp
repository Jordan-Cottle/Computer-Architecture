/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include <fstream>

#include "cpu.h"
#include "fetch.h"

#include "simulation.h"

using namespace Simulation;

Cpu::Cpu() : SimulationDevice("Cpu"),
             intRegister(Register<int>(REGISTER_COUNT, "Integer")),
             fpRegister(Register<float>(REGISTER_COUNT, "Float")),
             // Default settings for cpu0.s
             memory(new Memory(MEMORY_SIZE, MEMORY_DELAY, {0x200, 0x1400 - 0x200}))
{
    this->programCounter = ProgramCounter(MEMORY_ADDRESSES_PER_INSTRUCTION);
    this->branchSpeculated = false;
    this->jumpedFrom = -1;

    this->pipelines = {};
    this->complete = false;
    this->instructionsProcessed = 0;
}

Cpu::Cpu(Memory *memory) : SimulationDevice("Cpu"),
                           intRegister(Register<int>(REGISTER_COUNT, "Integer")),
                           fpRegister(Register<float>(REGISTER_COUNT, "Float")),
                           // Default settings for cpu0.s
                           memory(memory)
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
    int i = 0;
    for (auto pipeline : this->pipelines)
    {
        masterEventQueue.push(new Event("Tick", simulationClock.cycle, pipeline, MEDIUM + i));

        i += 2; // Leave space between pipeline priorities for other events to go
    }

    // Make sure any other events scheduled for this time are handled before main cpu tick
    masterEventQueue.push(new Event("Tick", simulationClock.cycle + SIM_CYCLES_PER_CPU, this, LOW));

    SimulationDevice::tick();
}

void Cpu::loadProgram(std::string fileName, uint32_t offset)
{
    std::ifstream programFile(fileName, std::ios::binary);

    if (!programFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint32_t instruction = 0;

    uint32_t memAddress = offset;
    while (programFile.read((char *)&instruction, sizeof(instruction)))
    {
        this->memory->write(memAddress, instruction);
        memAddress += sizeof(instruction);
    }

    this->programCounter.value = offset;
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

    s += "\t}\n\t" + addIndent(str(this->memory));

    s += "\t}\n\tPipelines: {\n";
    for (auto pipeline : this->pipelines)
    {
        s += "\t\t" + addIndent(str(pipeline), 2) + "\n";
    }

    s += "\t}\n}";

    return s;
}
