/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/

#ifndef __MEMORY_BUS__
#define __MEMORY_BUS__

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

// Forgive me for how I am going to use this
struct MesiEvent
{
    MesiSignal signal;
    uint32_t address;
    Cache *originator;

    MesiEvent(MesiSignal signal, uint32_t address, Cache *originator);
};

struct MemoryRequest : printable
{
    uint32_t address;
    SimulationDevice *device;
    uint32_t completeAt;
    bool read;

    MemoryRequest(uint32_t address, SimulationDevice *device, uint32_t completeAt, bool read);

    bool operator<(const MemoryRequest &);
    std::string __str__();
};

struct Cache;

// Use Memory Interface to keep it compatible with cpu0.s simulation
struct MemoryBus : MemoryInterface
{
    Memory *memory;

    MinHeap<MemoryRequest *> requests;
    std::vector<uint32_t> busyFor;
    std::vector<Cache *> caches;

    MemoryBus(int accessTime, Memory *memory);

    void linkCache(Cache *cache);
    void broadcast(MesiEvent *mesiEvent);
    bool trackedByOtherCache(uint32_t address, Cache *local);

    uint32_t port(uint32_t address);
    bool request(uint32_t address, SimulationDevice *device, bool read = true);
    void process(Event *event);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);

    std::string __str__();
};

#endif
