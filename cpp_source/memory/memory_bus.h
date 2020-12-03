/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/

#ifndef __MEMORY_BUS__
#define __MEMORY_BUS__

#include "sim_memory.h"
#include "heap.h"

struct MemoryRequest : printable
{
    uint32_t address;
    SimulationDevice *device;
    uint32_t completeAt;

    MemoryRequest(uint32_t address, SimulationDevice *device, uint32_t completeAt);

    bool operator<(const MemoryRequest &);
    std::string __str__();
};

// Use Memory Interface to keep it compatible with cpu0.s simulation
struct MemoryBus : MemoryInterface
{
    Memory *memory;

    MinHeap<MemoryRequest *> requests;
    std::vector<uint32_t> busyFor;

    MemoryBus(int accessTime, Memory *memory);

    uint32_t port(uint32_t address);
    bool request(uint32_t address, SimulationDevice *device);
    void process(Event *event);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);

    std::string __str__();
};

#endif
