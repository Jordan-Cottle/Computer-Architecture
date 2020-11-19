/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __FETCH__
#define __FETCH__

#include "pipeline.h"
#include "cpu.h"

struct Fetch;

struct Fetch : Pipeline
{
    Cpu *cpu;
    Fetch(Cpu *cpu);

    bool outstandingRequest;

    void tick();
    void process(Event *event);

    void processInstruction();
};

#endif
