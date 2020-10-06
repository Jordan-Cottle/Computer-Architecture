/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __FETCH__
#define __FETCH__

#include "event.h"
#include "pipeline.h"
#include "instruction_queue.h"
#include "sim_register.h"

struct Fetch;

struct FetchEvent : Event
{
    int address;
    FetchEvent(int time, Fetch *device, int address);

    std::string __str__();
};

struct Fetch : Pipeline
{
    Register<Instruction *> *source;
    Fetch(Pipeline *next, Register<Instruction *> *instructionMemory);

    void tick(EventQueue *eventQueue);
    void process(Event *event, EventQueue *eventQueue);
};

#endif