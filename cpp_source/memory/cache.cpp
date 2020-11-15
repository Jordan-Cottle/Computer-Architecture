/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#include "cache.h"

#include "binary.h"

#include "simulation.h"
using namespace Simulation;

Cache::Cache(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryInterface *source) : MemoryInterface(accessTime, size)
{
    if (size % blockSize != 0)
    {
        throw std::runtime_error("Block size of a cache must evenly divide it's entire size");
    }

    uint32_t blocks = size / blockSize;

    this->size = size;
    this->blockSize = blockSize;

    if (associativity == FULLY_ASSOCIATIVE)
    {
        this->associativity = blocks;
    }
    this->associativity = associativity;

    if (this->associativity > blocks)
    {
        throw std::runtime_error("Cache cannot have more associativity than it has blocks!");
    }

    this->data = new Memory(accessTime, size);

    this->offsetWidth = bitLength(blockSize - 1);
    this->offsetMask = slice(FULL_MASK, offsetWidth - 1, 0);

    this->blockIndexWidth = bitLength(blocks - 1);
    this->blockIndexMask = slice(FULL_MASK, blockIndexWidth + offsetWidth - 1, offsetWidth);

    this->tagWidth = 32 - blockIndexWidth - offsetWidth;
    this->tagMask = slice(FULL_MASK, 31, 32 - tagWidth);

    if ((this->offsetMask ^ this->blockIndexMask ^ this->tagMask) != FULL_MASK)
    {
        throw std::runtime_error("Mask computation for a mask has encountered an error!");
    }
}

uint32_t Cache::tag(uint32_t address)
{
    return (this->tagMask & address) >> (this->offsetWidth + this->blockIndexWidth);
}

uint32_t Cache::blockIndex(uint32_t address)
{
    return (this->blockIndexMask & address) >> this->offsetWidth;
}

uint32_t Cache::blockOffset(uint32_t address)
{
    return this->offsetMask & address;
}

bool Cache::request(uint32_t address, SimulationDevice *device)
{
    // TODO: Track tag, block, offset to compute local memory address
    bool accepted = this->data->request(address, this);

    if (!accepted)
    {
        throw std::runtime_error("Caches should only have one access at a time!");
    }

    this->requestor = device;

    return true;
}

void Cache::process(Event *event)
{
    if (event->type == "MemoryReady")
    {
        event->handled = true;
        Event *memoryReady = new Event(event->type, event->time, this->requestor, HIGH);
        masterEventQueue.push(memoryReady);
    }

    MemoryInterface::process(event);
}

uint32_t Cache::readUint(uint32_t address)
{
    return this->data->readUint(address);
}

int Cache::readInt(uint32_t address)
{
    return this->data->readInt(address);
}

float Cache::readFloat(uint32_t address)
{
    return this->data->readFloat(address);
}

void Cache::write(uint32_t address, uint32_t value)
{
    this->data->write(address, value);
}

void Cache::write(uint32_t address, int value)
{
    this->data->write(address, value);
}

void Cache::write(uint32_t address, float value)
{
    this->data->write(address, value);
}
