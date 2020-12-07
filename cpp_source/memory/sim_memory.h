/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#ifndef __MEMORY__
#define __MEMORY__

#include <vector>

#include "device.h"

#define MFMT(val) (void *)&val, sizeof(val)

struct MemoryInterface;
struct MemoryRequest : printable
{
    uint32_t address;
    SimulationDevice *device;
    bool read;
    bool inProgress;

    MemoryInterface *currentHandler;

    MemoryRequest(uint32_t address, SimulationDevice *device, bool read = true);

    void cancel();

    std::string __str__();
};

struct MemoryInterface : SimulationDevice
{
    uint32_t accessTime;
    uint32_t size;
    MemoryInterface(uint32_t accessTime, uint32_t size);

    bool withinBounds(uint32_t address);
    void checkBounds(uint32_t address);

    virtual bool request(MemoryRequest *request) = 0;
    virtual void cancelRequest(MemoryRequest *request) = 0;

    // I was using templates for this
    // But supporting using RAM directly and a Memory bus on the same cpu interface was impossible
    // I hate this. It's a large volume of code just for the sake of a compiler
    virtual uint32_t readUint(uint32_t address) = 0;
    virtual int readInt(uint32_t address) = 0;
    virtual float readFloat(uint32_t address) = 0;
    virtual void write(uint32_t address, void *start, uint32_t bytes) = 0;
};

struct MemoryBank : MemoryInterface
{
    std::vector<uint8_t> data;
    MemoryRequest *activeRequest;
    uint32_t logicalOffset;

    MemoryBank(uint32_t accessTime, uint32_t size, uint32_t logicalOffset);

    bool request(MemoryRequest *request);
    void cancelRequest(MemoryRequest *request);

    void process(Event *event);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);

    bool busy();
    uint32_t localAddress(uint32_t physicalAddress);

    std::string __str__();
};

struct MemoryController : MemoryInterface
{
    std::vector<MemoryBank *> memoryBanks;

    MemoryController(uint32_t accessTime, uint32_t size);
    MemoryController(uint32_t accessTime, uint32_t size, std::vector<uint32_t> partitions);

    uint32_t partition(uint32_t address);
    MemoryBank *getBank(uint32_t address);
    bool request(MemoryRequest *request);
    void cancelRequest(MemoryRequest *request);

    uint32_t readUint(uint32_t address);
    int readInt(uint32_t address);
    float readFloat(uint32_t address);
    void write(uint32_t address, void *start, uint32_t bytes);
};

#endif
