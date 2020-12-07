/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/

#ifndef __MEMORY_BUS__
#define __MEMORY_BUS__

#include <deque>

#include "sim_memory.h"
#include "heap.h"

enum MesiState
{
    MODIFIED,
    EXCLUSIVE,
    SHARED,
    INVALID
};

enum MesiSignal
{
    MEM_READ,
    RWITM,
    INVALIDATE
};

struct Cache;

struct MesiEvent : printable
{
    MesiSignal signal;
    uint32_t address;
    Cache *originator;

    MesiEvent(MesiSignal signal, uint32_t address, Cache *originator);

    std::string __str__();
};

std::string stateName(MesiState state);

struct Cache;

// Use Memory Interface to keep it compatible with cpu0.s simulation
struct MemoryBus : MemoryInterface
{
    MemoryController *memory;

    std::vector<std::deque<MemoryRequest *> *> requests;
    std::vector<Cache *> caches;

    MemoryBus(int accessTime, MemoryController *memory);

    void linkCache(Cache *cache);
    void broadcast(MesiEvent *mesiEvent);
    Cache *trackedBy(uint32_t address, Cache *local);

    uint32_t port(uint32_t address);
    bool request(MemoryRequest *request);
    void cancelRequest(MemoryRequest *request);

    void process(Event *event);
    MemoryRequest *findRequest(uint32_t address);
    void clearRequest(MemoryRequest *request);
    void clearRequest(uint32_t address);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);

    std::string __str__();
};

#endif
