/*
    Author: Jordan Cottle
    Created: 11/18/2020
*/

#ifndef __MEMORY_ROUTER__
#define __MEMORY_ROUTER__

#include "cache.h"

struct MemoryRouter : MemoryInterface
{
    MemoryInterface *instrutionCache;
    MemoryInterface *dataCache;

    std::unordered_map<uint32_t, MemoryInterface *> memoryMap;

    MemoryRouter(MemoryInterface *instrutionCache, MemoryInterface *dataCache);

    MemoryInterface *selectMemoryDevice(SimulationDevice *device);
    MemoryInterface *selectMemoryDevice(uint32_t address);

    bool request(uint32_t address, SimulationDevice *device);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, uint32_t value);
    void write(uint32_t address, int value);
    void write(uint32_t address, float value);
};

#endif