/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#ifndef __CACHE__
#define __CACHE__

#include "sim_memory.h"

constexpr uint32_t DIRECT_MAPPED = 1;
constexpr uint32_t FULLY_ASSOCIATIVE = 0;

struct Cache : MemoryInterface
{
    // Configurable attributes
    uint32_t size;
    uint32_t blockSize;
    uint32_t associativity;

    // Attributes
    Memory *data;
    std::vector<bool> valid;
    SimulationDevice *requestor;

    uint32_t tagWidth;
    uint32_t tagMask;
    uint32_t blockIndexWidth;
    uint32_t blockIndexMask;
    uint32_t offsetWidth;
    uint32_t offsetMask;

    // Statistics
    uint32_t accesses;
    uint32_t hits;
    uint32_t misses;

    Cache(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryInterface *source);

    uint32_t tag(uint32_t address);
    uint32_t blockIndex(uint32_t address);
    uint32_t blockOffset(uint32_t address);

    bool request(uint32_t address, SimulationDevice *device);

    void process(Event *event);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, uint32_t value);
    void write(uint32_t address, int value);
    void write(uint32_t address, float value);
};

#endif
