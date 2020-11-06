/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __DECODE__
#define __DECODE__

#include "pipeline.h"

#include "cpu.h"

struct Decode : Pipeline
{
    Cpu *cpu;

    Decode(Cpu *cpu);

    DecodedInstruction *decode(RawInstruction *instruction);

    void tick();
};

#endif
