/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#include "cache.h"

#include "binary.h"

#include "simulation.h"
using namespace Simulation;

AddressNotFound::AddressNotFound(uint32_t address) : std::runtime_error("Memory address " + str(address) + " not available in cache")
{
    this->address = address;
}

Cache::Cache(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryInterface *source) : MemoryInterface(accessTime, size)
{
    this->source = source;
    this->outstandingMiss = false;

    if (size % blockSize != 0)
    {
        throw std::logic_error("Block size of a cache must evenly divide it's entire size");
    }

    uint32_t blocks = size / blockSize;
    this->valid = std::vector<bool>(blocks);
    this->tags = std::vector<uint32_t>(blocks);
    this->lruBits = std::vector<bool>(blocks);

    this->blockSize = blockSize;

    if (associativity == FULLY_ASSOCIATIVE)
    {
        this->associativity = blocks;
    }
    this->associativity = associativity;

    if (this->associativity > blocks)
    {
        throw std::logic_error("Cache cannot have more associativity than it has blocks!");
    }
    uint32_t sets = blocks / associativity;

    this->data = new Memory(accessTime, size);

    this->offsetWidth = bitLength(blockSize - 1);
    this->offsetMask = slice(FULL_MASK, offsetWidth - 1, 0);

    this->indexWidth = bitLength(sets - 1);
    this->indexMask = slice(FULL_MASK, indexWidth + offsetWidth - 1, offsetWidth);

    this->tagWidth = 32 - indexWidth - offsetWidth;
    this->tagMask = slice(FULL_MASK, 31, 32 - tagWidth);

    if ((this->offsetMask ^ this->indexMask ^ this->tagMask) != FULL_MASK)
    {
        throw std::logic_error("Mask computation for a mask has encountered an error!");
    }

    this->hits = 0;
    this->misses = 0;
    this->accesses = 0;
}

uint32_t Cache::tag(uint32_t address)
{
    return (this->tagMask & address) >> (this->offsetWidth + this->indexWidth);
}

uint32_t Cache::index(uint32_t address)
{
    return (this->indexMask & address) >> this->offsetWidth;
}

uint32_t Cache::offset(uint32_t address)
{
    return this->offsetMask & address;
}

uint32_t Cache::cacheAddress(uint32_t address)
{
    uint32_t tag = this->tag(address);
    uint32_t index = this->index(address);
    uint32_t offset = this->offset(address);

    uint32_t startBlockIndex = index * this->associativity;

    for (uint32_t i = 0; i < this->associativity; i++)
    {
        uint32_t blockIndex = startBlockIndex + i;
        if (!this->valid[blockIndex])
        {
            continue;
        }

        if (this->tags[blockIndex] == tag)
        {
            return ((blockIndex * this->blockSize) + offset);
        }
    }

    throw AddressNotFound(address);
}

void Cache::updateLruState(uint32_t address)
{
    // Get actual block index of location in cache
    uint32_t tag = this->tag(address);
    uint32_t index = this->index(address);
    uint32_t startBlockIndex = index * this->associativity;
    uint32_t blockIndex;

    bool found = false;
    for (uint32_t i = 0; i < this->associativity; i++)
    {
        blockIndex = startBlockIndex + i;
        if (this->tags[blockIndex] == tag)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        throw std::logic_error("Updates to lru state can only be applied to blocks that are in the cache!");
    }

    std::cout << "Address " << address << " at cache block " << blockIndex << " in set " << index << " has been used\n";

    // Set bit if not set
    if (!this->lruBits[blockIndex])
    {
        this->lruBits[blockIndex] = true;
        return;
    }

    // Bit was already set, clear all other bits
    for (uint32_t i = 0; i < this->associativity; i++)
    {
        this->lruBits[index + i] = false;
    }

    // Reset current bit to true
    this->lruBits[blockIndex] = true;
}

uint32_t Cache::blockToEvict(uint32_t address)
{
    uint32_t tag = this->tag(address);
    uint32_t index = this->index(address);
    uint32_t startBlockIndex = index * this->associativity;
    uint32_t blockIndex;

    for (uint32_t i = 0; i < this->associativity; i++)
    {
        blockIndex = startBlockIndex + i;
        // Select first available invalid block
        if (!this->valid[blockIndex])
        {
            return blockIndex;
        }
        else if (this->tags[blockIndex] == tag)
        {
            std::cout << "Cache reloading block " << blockIndex << "\n";
            return blockIndex;
        }
    }

    // Select first LRU unset location
    for (uint32_t i = 0; i < this->associativity; i++)
    {
        if (!this->lruBits[startBlockIndex + i])
        {
            blockIndex = startBlockIndex + i;
            break;
        }
    }

    return blockIndex;
}

void Cache::loadBlock(uint32_t address)
{
    uint32_t blockIndex = this->blockToEvict(address);
    std::cout << "Set " << this->index(address) << " replacing block " << blockIndex % this->associativity << "\n";

    // Shouldn't this take some time?
    uint32_t start = blockIndex * this->blockSize;
    for (uint32_t i = 0; i < this->blockSize; i += 4)
    {
        this->data->write(start + i, this->source->readUint(address + i));
    }

    this->valid[blockIndex] = true;
    this->tags[blockIndex] = this->tag(address);
}

bool Cache::request(uint32_t address, SimulationDevice *device)
{
    uint32_t cacheAddress;
    this->requestor = device;
    bool accepted;
    try
    {
        cacheAddress = this->cacheAddress(address);
    }
    catch (AddressNotFound &error)
    {
        this->misses++;
        // Request block from memory
        accepted = this->source->request(address, this);
        this->outstandingMiss = true;
        this->addressRequested = address;
        if (!accepted)
        {
            throw std::logic_error("Caches do not handle downstream rejecting it's memory request properly!");
        }
        return accepted;
    }

    accepted = this->data->request(cacheAddress, this);

    if (!accepted)
    {
        throw std::logic_error("Caches should only have one access at a time!");
    }

    return true;
}

void Cache::process(Event *event)
{
    if (event->type == "MemoryReady")
    {
        event->handled = true;

        if (this->outstandingMiss)
        {
            this->loadBlock(this->addressRequested);
            this->outstandingMiss = false;
            this->request(this->addressRequested, this->requestor); // Re trigger request
        }
        else
        {
            Event *memoryReady = new Event(event->type, event->time, this->requestor, HIGH);
            masterEventQueue.push(memoryReady);
        }
    }

    MemoryInterface::process(event);
}

uint32_t Cache::readUint(uint32_t address)
{
    this->updateLruState(address);
    return this->data->readUint(this->cacheAddress(address));
}

int Cache::readInt(uint32_t address)
{
    this->updateLruState(address);
    return this->data->readInt(this->cacheAddress(address));
}

float Cache::readFloat(uint32_t address)
{
    this->updateLruState(address);
    return this->data->readFloat(this->cacheAddress(address));
}

void Cache::write(uint32_t address, uint32_t value)
{
    this->updateLruState(address);
    this->data->write(this->cacheAddress(address), value);
}

void Cache::write(uint32_t address, int value)
{
    this->updateLruState(address);
    this->data->write(this->cacheAddress(address), value);
}

void Cache::write(uint32_t address, float value)
{
    this->updateLruState(address);
    this->data->write(this->cacheAddress(address), value);
}
