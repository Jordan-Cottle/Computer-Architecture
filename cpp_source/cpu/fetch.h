/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __FETCH__
#define __FETCH__

#include "pipeline.h"
#include "cpu.h"

struct Fetch;

struct FetchEvent : Event
{
    int address;
    FetchEvent(ulong time, Fetch *device, int address);

    std::string __str__();
};

struct Fetch : Pipeline
{
    Cpu *cpu;
    Fetch(Cpu *cpu);

    void tick(ulong time, EventQueue *eventQueue);
    void process(Event *event, EventQueue *eventQueue);
};

#endif
