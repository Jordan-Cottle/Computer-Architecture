/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __CPU__
#define __CPU__

#include "pipeline.h"
#include "instruction_queue.h"

struct Cpu : SimulationDevice
{
    int programCounter;

    Register<int> intRegister;
    Register<double> floatRegister;

    // TODO put these somewhere else
    Register<Instruction *> instructionMemory;
    Register<int> intMemory;
    Register<double> fpMemory;

    std::vector<Pipeline *> pipelines;

    Cpu();
    // Cpu(InstructionQueue *instructionSource);

    Cpu *addPipeline(Pipeline *pipeline);

    void tick(ulong time, EventQueue *eventQueue);

    std::string __str__();
};

#endif
