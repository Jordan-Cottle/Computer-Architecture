/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/

#ifndef __MEMORY_BUS__
#define __MEMORY_BUS__

#include "sim_memory.h"
#include "heap.h"

constexpr uint32_t BUS_ARBITRATION_TIME = 5;

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
struct MemoryBus : Memory
{
    Memory *memory;

    MinHeap<MemoryRequest *> requests;
    std::vector<uint32_t> busyFor;

    MemoryBus(int accessTime, Memory *memory);

    uint32_t port(uint32_t address);
    bool request(uint32_t address, SimulationDevice *device);
    void process(Event *event);

    template <typename T>
    T read(uint32_t address)
    {
        return this->memory->read<T>(address);
    }

    template <typename T>
    void write(uint32_t address, T value)
    {
        this->memory->write(address, value);
    }

    std::string __str__();
};

#endif
