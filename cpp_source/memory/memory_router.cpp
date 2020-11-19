/*
    Author: Jordan Cottle
    Created: 11/18/2020
*/

#include "memory_router.h"

MemoryRouter::MemoryRouter(MemoryInterface *instrutionCache, MemoryInterface *dataCache) : MemoryInterface(0, 0)
{
    this->memoryMap = std::unordered_map<uint32_t, MemoryInterface *>();

    this->instrutionCache = instrutionCache;
    this->dataCache = dataCache;
}

MemoryInterface *MemoryRouter::selectMemoryDevice(SimulationDevice *device)
{
    if (device->type == "Fetch")
    {
        return this->instrutionCache;
    }

    return this->dataCache;
}

MemoryInterface *MemoryRouter::selectMemoryDevice(uint32_t address)
{
    return this->memoryMap[address];
}

bool MemoryRouter::request(uint32_t address, SimulationDevice *device)
{
    MemoryInterface *memoryDevice = this->selectMemoryDevice(device);
    this->memoryMap[address] = memoryDevice;
    return memoryDevice->request(address, device);
}

uint32_t MemoryRouter::readUint(uint32_t address)
{
    return this->selectMemoryDevice(address)->readUint(address);
}

int MemoryRouter::readInt(uint32_t address)
{
    return this->selectMemoryDevice(address)->readInt(address);
}

float MemoryRouter::readFloat(uint32_t address)
{
    return this->selectMemoryDevice(address)->readFloat(address);
}

void MemoryRouter::write(uint32_t address, uint32_t value)
{
    this->selectMemoryDevice(address)->write(address, value);
}

void MemoryRouter::write(uint32_t address, int value)
{
    this->selectMemoryDevice(address)->write(address, value);
}

void MemoryRouter::write(uint32_t address, float value)
{
    this->selectMemoryDevice(address)->write(address, value);
}
