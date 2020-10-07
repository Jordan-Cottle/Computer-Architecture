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

struct Pipeline;

struct PipelineInsertEvent : Event
{
    Instruction *instruction;

    PipelineInsertEvent(ulong time, Pipeline *device, Instruction *instruction);
};

struct PipelineFlushEvent : Event
{
    PipelineFlushEvent(ulong time, Pipeline *device);
};

struct Pipeline : SimulationDevice
{
    Register<Instruction *> memory;
    Pipeline *next;

    Pipeline(std::string type);
    Pipeline(std::string type, Pipeline *next);

    bool free();

    void stage(Instruction *);
    void flush();

    Instruction *staged();

    void tick(ulong time, EventQueue *eventQueue);
    void process(Event *event, EventQueue *eventQueue);

    std::string __str__();
};

#endif
