/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __CPU__
#define __CPU__

#include "pipeline.h"
#include "instruction.h"
#include "sim_memory.h"

#include "program.h"
#include "program_counter.h"

struct Cpu : SimulationDevice
{
    ProgramCounter programCounter;
    bool branchSpeculated;
    int jumpedFrom;

    bool complete;

    Register<int> intRegister;
    Register<float> fpRegister;

    // TODO put these somewhere else
    Program *program;
    Register<float> fpMemory;
    Memory ram;

    std::vector<Pipeline *> pipelines;

    Cpu();

    Cpu *addPipeline(Pipeline *pipeline);

    void tick();

    void loadProgram(Program *program);

    void flush();

    Pipeline *getPipeline(std::string type);

    std::string __str__();
};

#endif
