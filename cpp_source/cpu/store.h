/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#ifndef __STORE__
#define __STORE__

#include "pipeline.h"
#include "sim_memory.h"

struct StorePipeline : Pipeline
{
    MemoryRequest *activeRequest;
    Cpu *cpu;
    StorePipeline(Cpu *cpu);

    void tick();
    void process(Event *event);
};

#endif
