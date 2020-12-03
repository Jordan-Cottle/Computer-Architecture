/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#ifndef __CACHE__
#define __CACHE__

#include "sim_memory.h"

constexpr uint32_t DIRECT_MAPPED = 1;
constexpr uint32_t FULLY_ASSOCIATIVE = 0;

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

struct AddressNotFound : std::runtime_error
{
    uint32_t address;
    AddressNotFound(uint32_t address);
};

struct Cache : MemoryInterface
{
    // Configurable attributes
    uint32_t blockSize;
    uint32_t associativity;

    // Attributes
    Memory *data;
    std::vector<bool> valid;
    std::vector<uint32_t> tags;
    std::vector<bool> lruBits;
    std::vector<MesiState> mesiStates;

    uint32_t tagWidth;
    uint32_t tagMask;
    uint32_t indexWidth;
    uint32_t indexMask;
    uint32_t offsetWidth;
    uint32_t offsetMask;

    // Statistics
    uint32_t accesses;
    uint32_t hits;
    uint32_t compulsoryMisses;
    std::vector<uint32_t> seen;

    // Backing MemorySource
    MemoryInterface *source;

    // Request tracking data
    bool outstandingMiss;
    uint32_t addressRequested;
    SimulationDevice *requestor;

    Cache(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryInterface *source);

    uint32_t tag(uint32_t address);
    uint32_t index(uint32_t address);
    uint32_t offset(uint32_t address);
    uint32_t findBlock(uint32_t address);
    uint32_t cacheAddress(uint32_t address);

    void updateLruState(uint32_t address);
    uint32_t blockToEvict(uint32_t index);
    void loadBlock(uint32_t address);

    bool request(uint32_t address, SimulationDevice *device, bool read = true);
    bool request(uint32_t address, SimulationDevice *device, bool read, bool reIssued);
    void process(Event *event);

    bool snoop(MesiSignal signal, uint32_t address, Cache *sender);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);
};

#endif
