/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#ifndef __STORE__
#define __STORE__

#include "pipeline.h"

struct StorePipeline : Pipeline
{

    Cpu *cpu;
    StorePipeline(Cpu *cpu);

    void tick();
};

#endif
