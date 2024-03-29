/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#ifndef __EXECUTE__
#define __EXECUTE__

#include "pipeline.h"

#include "cpu.h"

struct Execute : Pipeline
{

    MemoryRequest *activeRequest;
    Cpu *cpu;
    Execute(Cpu *cpu);

    void tick();
    void process(Event *event);
};

#endif
