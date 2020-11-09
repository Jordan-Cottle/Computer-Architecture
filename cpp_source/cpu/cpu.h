/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __CPU__
#define __CPU__

#include "pipeline.h"
#include "instruction.h"
#include "sim_memory.h"

#include "program_counter.h"

struct Cpu : SimulationDevice
{
    ProgramCounter programCounter;
    bool branchSpeculated;
    int jumpedFrom;

    bool complete;

    uint32_t instructionsProcessed;

    Register<int> intRegister;
    Register<float> fpRegister;

    // TODO put these somewhere else
    Memory memory;

    std::vector<Pipeline *> pipelines;

    Cpu();

    Cpu *addPipeline(Pipeline *pipeline);

    void tick();
    void process(Event *event);

    void loadProgram(std::string fileName);

    void flush();

    Pipeline *getPipeline(std::string type);

    float cpi();
    std::string __str__();
};

#endif
