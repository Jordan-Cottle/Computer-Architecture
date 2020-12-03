/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#ifndef __MEMORY__
#define __MEMORY__

#include <vector>

#include "device.h"

struct MemoryInterface : SimulationDevice
{
    uint32_t accessTime;
    uint32_t size;
    MemoryInterface(uint32_t accessTime, uint32_t size);

    bool withinBounds(uint32_t address);
    void checkBounds(uint32_t address);

    virtual bool request(uint32_t address, SimulationDevice *device) = 0;

    // I was using templates for this
    // But supporting using RAM directly and a Memory bus on the same cpu interface was impossible
    // I hate this. It's a large volume of code just for the sake of a compiler
    virtual uint32_t readUint(uint32_t address) = 0;
    virtual int readInt(uint32_t address) = 0;
    virtual float readFloat(uint32_t address) = 0;
    virtual void write(uint32_t address, void *start, uint32_t bytes) = 0;
};

struct Memory : MemoryInterface
{
    std::vector<uint8_t> data;
    std::vector<uint32_t> partitions;
    std::vector<bool> busy;

    Memory(uint32_t accessTime, uint32_t size);
    Memory(uint32_t accessTime, uint32_t size, std::vector<uint32_t> partitions);

    uint32_t partition(uint32_t address);
    bool request(uint32_t address, SimulationDevice *device);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);

    std::string __str__();
};

#endif
