/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __CPU__
#define __CPU__

#include "pipeline.h"
#include "instruction.h"

#include "program.h"

struct Cpu : SimulationDevice
{
    int programCounter;

    Register<int> intRegister;
    Register<double> fpRegister;

    // TODO put these somewhere else
    Register<Instruction *> instructionMemory;
    Register<int> intMemory;
    Register<double> fpMemory;

    std::vector<Pipeline *> pipelines;

    Cpu();

    Cpu *addPipeline(Pipeline *pipeline);

    void tick(ulong time, EventQueue *eventQueue);

    void loadProgram(Program *program);

    std::string __str__();
};

#endif
