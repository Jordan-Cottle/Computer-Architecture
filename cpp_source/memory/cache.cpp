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
    this->mesiStates = std::vector<MesiState>(blocks);

    for (uint32_t i = 0; i < blocks; i++)
    {
        mesiStates.at(i) = INVALID;
    }

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
    this->compulsoryMisses = 0;
    this->accesses = 0;
    this->seen = std::vector<uint32_t>();
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

uint32_t Cache::findBlock(uint32_t address)
{
    uint32_t tag = this->tag(address);
    uint32_t index = this->index(address);
    uint32_t startBlockIndex = index * this->associativity;

    for (uint32_t i = 0; i < this->associativity; i++)
    {
        uint32_t blockIndex = startBlockIndex + i;
        if (this->valid[blockIndex] && this->tags[blockIndex] == tag)
        {
            return blockIndex;
        }
    }

    throw AddressNotFound(address);
}

uint32_t Cache::cacheAddress(uint32_t address)
{
    uint32_t offset = this->offset(address);

    uint32_t blockIndex = this->findBlock(address);
    return ((blockIndex * this->blockSize) + offset);
}

void Cache::updateLruState(uint32_t address)
{
    // Get actual block index of location in cache
    uint32_t index = this->index(address);
    uint32_t blockIndex = this->findBlock(address);

    OUT << "Address " << address << " at cache block " << blockIndex << " in set " << index << " has been used\n";

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
    uint32_t blockIndex = 0;

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
            OUT << "Cache reloading block " << blockIndex << "\n";
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
    OUT << "Set " << this->index(address) << " replacing block " << blockIndex % this->associativity << "\n";

    uint32_t memoryStart = address ^ this->offset(address);
    uint32_t start = blockIndex * this->blockSize;
    for (uint32_t i = 0; i < this->blockSize; i += 4)
    {
        // Shouldn't this take some time?
        uint32_t data = this->source->readUint(memoryStart + i);
        this->data->write(start + i, (void *)&data, sizeof(data));
    }

    this->valid[blockIndex] = true;
    this->tags[blockIndex] = this->tag(address);
}

bool Cache::request(uint32_t address, SimulationDevice *device, bool read)
{
    this->accesses += 1;
    return this->request(address, device, read, false);
}

bool Cache::request(uint32_t address, SimulationDevice *device, bool read, bool reIssued)
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
        if (reIssued)
        {
            throw std::logic_error("Reissued requests should never result in a miss!");
        }

        // Check if this block has been seen before
        bool seen = false;
        uint32_t memBlock = address ^ (this->offset(address));
        for (auto seenBlock : this->seen)
        {
            if (memBlock == seenBlock)
            {
                seen = true;
                break;
            }
        }

        if (!seen)
        {
            this->compulsoryMisses++;
            this->seen.push_back(memBlock);
        }

        // Request block from memory
        accepted = this->source->request(address, this, read);
        this->outstandingMiss = true;
        this->addressRequested = address;
        if (!accepted)
        {
            throw std::logic_error("Caches do not handle downstream rejecting it's memory request properly!");
        }
        return accepted;
    }

    accepted = this->data->request(cacheAddress, this, read);

    if (!accepted)
    {
        throw std::logic_error("Caches should only have one access at a time!");
    }

    if (!reIssued)
    {
        this->hits += 1;
    }

    return true;
}

void Cache::process(Event *event)
{
    if (event->type == "MemoryReadReady" || event->type == "MemoryWriteReady")
    {
        event->handled = true;

        if (this->outstandingMiss)
        {
            this->loadBlock(this->addressRequested);
            this->outstandingMiss = false;
            this->request(this->addressRequested, this->requestor, event->type == "MemoryReadReady", true); // Re trigger request
        }
        else
        {
            Event *memoryReady = new Event(event->type, event->time, this->requestor, HIGH);
            masterEventQueue.push(memoryReady);
        }
    }

    MemoryInterface::process(event);
}

bool Cache::snoop(MesiSignal signal, uint32_t address, Cache *sender)
{
    uint32_t blockIndex;
    try
    {
        blockIndex = this->findBlock(address);
    }
    catch (AddressNotFound &error)
    {
        return false; // Cache is not tracking that address
    }

    MesiState state = this->mesiStates.at(blockIndex);

    if (state == INVALID)
    {
        return false;
    }

    switch (signal)
    {
    case INVALIDATE:
        this->mesiStates.at(blockIndex) = INVALID;
        this->valid.at(blockIndex) = false;
        break;
    case MEM_READ:
        switch (state)
        {
        case MODIFIED:
            // TODO write back to memory
        case EXCLUSIVE:
        case SHARED:
            this->mesiStates.at(blockIndex) = SHARED;
            // TODO send memory to requestor
        default:
            break;
        }
        break;
    case RWITM:
        switch (state)
        {
        case MODIFIED:
            // TODO write back to memory
        case EXCLUSIVE:
        case SHARED:
            // TODO send memory to requestor
            this->mesiStates.at(blockIndex) = INVALID;
            this->valid.at(blockIndex) = false;
        default:
            break;
        }
    }

    return true;
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

void Cache::write(uint32_t address, void *start, uint32_t bytes)
{
    this->updateLruState(address);
    this->data->write(this->cacheAddress(address), start, bytes);
    this->source->write(address, start, bytes);
}
