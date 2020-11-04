/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __PIPELINE__
#define __PIPELINE__

#include "event_queue.h"
#include "device.h"
#include "sim_register.h"
#include "instruction.h"

struct Pipeline : SimulationDevice
{
    Register<RawInstruction *> memory;
    Pipeline *next;

    Pipeline(std::string type);
    Pipeline(std::string type, Pipeline *next);

    bool free();

    void stage(RawInstruction *);
    void flush();

    RawInstruction *staged();

    void tick();

    std::string __str__();
};

#endif
