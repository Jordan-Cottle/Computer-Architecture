/*
    Author: Jordan Cottle
    Created: 11/18/2020
*/

#include "memory_router.h"

MemoryRouter::MemoryRouter(MemoryInterface *instrutionCache, MemoryInterface *dataCache) : MemoryInterface(0, 0)
{
    this->instrutionCache = instrutionCache;
    this->dataCache = dataCache;
}

MemoryInterface *MemoryRouter::selectMemoryDevice(uint32_t address)
{
    if (address < 0x200)
    {
        return this->instrutionCache;
    }

    return this->dataCache;
}

bool MemoryRouter::request(MemoryRequest *request)
{
    MemoryInterface *memoryDevice = this->selectMemoryDevice(request->address);
    return memoryDevice->request(request);
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

void MemoryRouter::write(uint32_t address, void *start, uint32_t bytes)
{
    this->selectMemoryDevice(address)->write(address, start, bytes);
}
