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

    this->data = new Memory(accessTime, size);

    this->offsetWidth = bitLength(blockSize - 1);
    this->offsetMask = slice(FULL_MASK, offsetWidth - 1, 0);

    this->blockIndexWidth = bitLength(blocks - 1);
    this->blockIndexMask = slice(FULL_MASK, blockIndexWidth + offsetWidth - 1, offsetWidth);

    this->tagWidth = 32 - blockIndexWidth - offsetWidth;
    this->tagMask = slice(FULL_MASK, 31, 32 - tagWidth);

    if ((this->offsetMask ^ this->blockIndexMask ^ this->tagMask) != FULL_MASK)
    {
        throw std::logic_error("Mask computation for a mask has encountered an error!");
    }

    this->hits = 0;
    this->misses = 0;
    this->accesses = 0;
}

uint32_t Cache::tag(uint32_t address)
{
    return (this->tagMask & address) >> (this->offsetWidth + this->blockIndexWidth);
}

uint32_t Cache::blockIndex(uint32_t address)
{
    return (this->blockIndexMask & address) >> this->offsetWidth;
}

uint32_t Cache::setIndex(uint32_t address)
{
    return this->blockIndex(address) / this->associativity;
}

uint32_t Cache::blockOffset(uint32_t address)
{
    return this->offsetMask & address;
}

uint32_t Cache::cacheAddress(uint32_t address)
{
    uint32_t tag = this->tag(address);
    uint32_t setIndex = this->setIndex(address);
    uint32_t offset = this->blockOffset(address);

    uint32_t startBlockIndex = setIndex * this->associativity;

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

void Cache::loadBlock(uint32_t address)
{
    uint32_t tag = this->tag(address);
    uint32_t setIndex = this->setIndex(address);

    uint32_t blockIndex = setIndex * this->associativity;
    bool spaceFound = false;
    for (uint32_t i = 0; i < this->associativity; i++)
    {
        // Select first available invalid block
        if (!this->valid[blockIndex])
        {
            std::cout << "Cache allocating fresh space for address: " << str(address) << "\n";
            spaceFound = true;
        }
        else if (this->tags[blockIndex] == tag)
        {
            std::cout << "Cache reloading address: " << str(address) << "\n";
            spaceFound = true;
        }
        else
        {
            blockIndex++;
        }

        if (spaceFound)
        {
            break;
        }
    }

    if (!spaceFound)
    {
        // TODO handle replacement policy
        throw std::runtime_error("No free space in cache found and no replacement policy implementation yet!");
    }

    // Shouldn't this take some time?
    uint32_t start = blockIndex * this->blockSize;
    for (uint32_t i = 0; i < this->blockSize; i += 4)
    {
        this->data->write(start + i, this->source->readUint(address + i));
    }

    this->valid[blockIndex] = true;
    this->tags[blockIndex] = tag;
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
    return this->data->readUint(this->cacheAddress(address));
}

int Cache::readInt(uint32_t address)
{
    return this->data->readInt(this->cacheAddress(address));
}

float Cache::readFloat(uint32_t address)
{
    return this->data->readFloat(this->cacheAddress(address));
}

void Cache::write(uint32_t address, uint32_t value)
{
    this->data->write(this->cacheAddress(address), value);
}

void Cache::write(uint32_t address, int value)
{
    this->data->write(this->cacheAddress(address), value);
}

void Cache::write(uint32_t address, float value)
{
    this->data->write(this->cacheAddress(address), value);
}
