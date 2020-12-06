/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#include <math.h>

#include "cache.h"

#include "binary.h"

#include "simulation.h"
using namespace Simulation;

AddressNotFound::AddressNotFound(uint32_t address) : std::runtime_error("Memory address " + str(address) + " not available in cache")
{
    this->address = address;
}

WriteBack::WriteBack(uint32_t address, Cache *cache) : std::runtime_error(str(cache) + " needs to write " + str(address) + " back to memory")
{
    this->address = address;
    this->cache = cache;
}

Cache::Cache(uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryBus *source) : MemoryInterface(uint32_t(ceil(log2(size / float(blockSize)))) * associativity, size)
{
    initialize(this->accessTime, size, blockSize, associativity, source);
}

Cache::Cache(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryBus *source) : MemoryInterface(accessTime, size)
{
    initialize(accessTime, size, blockSize, associativity, source);
}

void Cache::initialize(uint32_t accessTime, uint32_t size, uint32_t blockSize, uint32_t associativity, MemoryBus *source)
{
    this->type = "Cache";
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
    this->previousMesiStates = std::vector<MesiState>(blocks);
    this->memoryAddresses = std::vector<uint32_t>(blocks);

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

    this->source->linkCache(this);
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

    DEBUG << "Address " << address << " at cache block " << blockIndex << " in set " << index << " has been used\n";

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
            // This probably should never happen
            WARNING << str(this) << " reloading block " << blockIndex << "\n";
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
    DEBUG << str(this) << " replacing block " << blockIndex << " in set " << this->index(address) << " to handle memory address " << str(address) << "\n";

    if (this->valid.at(blockIndex) && this->previousMesiStates.at(blockIndex) == MODIFIED)
    {

        uint32_t previousAddress = this->memoryAddresses.at(blockIndex);
        DEBUG << str(this) << " writing back block " << blockIndex << " in set " << this->index(address) << " to memory address " << str(previousAddress) << " to make room for " << str(address) << "\n";
        this->writeBackBlock(blockIndex, previousAddress);
    }

    uint32_t memoryStart = address ^ this->offset(address);
    uint32_t start = blockIndex * this->blockSize;
    for (uint32_t i = 0; i < this->blockSize; i += 4)
    {
        // Shouldn't this take some time?
        uint32_t data = this->source->readUint(memoryStart + i);
        this->data->write(start + i, MFMT(data));
    }

    this->valid.at(blockIndex) = true;
    this->tags.at(blockIndex) = this->tag(address);
    this->memoryAddresses.at(blockIndex) = address;
}

void Cache::writeBackBlock(uint32_t blockIndex, uint32_t address)
{
    uint32_t cacheStart = blockIndex * this->blockSize;
    DEBUG << "Writing back starting from cache address " << str(cacheStart) << "\n";

    uint32_t memoryStart = address ^ this->offset(address);
    for (uint32_t i = 0; i < this->blockSize; i += 4)
    {
        float data = this->data->readFloat(cacheStart + i);
        DEBUG << "Writing back " << str(data) << " into memory at " << str(memoryStart + i) << "\n";
        this->source->write(memoryStart + i, MFMT(data));
    }
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

    DEBUG << str(this) << " received " << (read ? "read" : "write") << " request for address " << str(address) << "\n";
    try
    {
        cacheAddress = this->cacheAddress(address);
    }
    catch (AddressNotFound &error) // MISS
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

        if (read)
        {
            // read miss
            if (this->source->trackedBy(address, this) != NULL)
            {
                this->setState(address, SHARED);
            }
            else
            {
                this->setState(address, EXCLUSIVE);
            }
            this->source->broadcast(new MesiEvent(MEM_READ, address, this));
        }
        else
        {
            // write miss
            Cache *tracking = this->source->trackedBy(address, this);
            if (tracking == NULL)
            {
                // No other cache watching this address
                this->setState(address, MODIFIED);
            }
            else
            {
                switch (tracking->state(address))
                {
                case EXCLUSIVE:
                case SHARED:
                    this->source->broadcast(new MesiEvent(RWITM, address, this));
                    this->setState(address, MODIFIED);
                    break;
                case MODIFIED:
                    this->source->broadcast(new MesiEvent(RWITM, address, this));
                    this->setState(address, MODIFIED);
                    break;
                default:
                    throw std::logic_error("Write miss not handled yet!");
                }
            }
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
        // HIT
        this->hits += 1;
        if (!read)
        {
            // write hit
            switch (this->state(address))
            {
            case SHARED:
                this->source->broadcast(new MesiEvent(INVALIDATE, address, this));
            case EXCLUSIVE:
                this->setState(address, MODIFIED);
                break;
            case INVALID:
                throw std::logic_error("Cannot have a write hit with an invalid mesi state!");
                break;
            default:
                break;
            }
        }
    }

    return accepted;
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
    else if (event->type == "WriteBack")
    {
        event->handled = true;
        uint32_t blockIndex = findBlock(this->writeBackAddress);
        this->writeBackBlock(blockIndex, this->writeBackAddress);

        this->setState(this->writeBackAddress, this->writeBackState);
    }

    MemoryInterface::process(event);
}

bool Cache::snoop(MesiEvent *mesiEvent)
{
    uint32_t address = mesiEvent->address;

    MesiState state;
    try
    {
        state = this->state(address);
    }
    catch (AddressNotFound &error)
    {
        return false; // Cache is not tracking that address
    }

    if (state == INVALID)
    {
        return false;
    }

    switch (mesiEvent->signal)
    {
    case INVALIDATE:
        this->setState(address, INVALID);
        break;
    case MEM_READ:
        switch (state)
        {
        case MODIFIED:
            this->writeBackAddress = address;
            this->writeBackState = SHARED;
            throw new WriteBack(mesiEvent->address, this);
        case EXCLUSIVE:
        case SHARED:
            this->setState(address, SHARED);
        default:
            break;
        }
        break;
    case RWITM:
        switch (state)
        {
        case MODIFIED:
            this->writeBackAddress = mesiEvent->address;
            this->writeBackState = INVALID;
            throw new WriteBack(mesiEvent->address, this);
        case EXCLUSIVE:
        case SHARED:
            this->setState(address, INVALID);
        default:
            break;
        }
    }

    return true;
}

MesiState Cache::state(uint32_t address)
{
    try
    {
        return this->mesiStates.at(this->findBlock(address));
    }
    catch (AddressNotFound &err)
    {
        return INVALID;
    }
}

std::string stateName(MesiState state)
{
    std::string stateName;
    switch (state)
    {
    case MODIFIED:
        stateName = "MODIFIED";
        break;
    case EXCLUSIVE:
        stateName = "EXCLUSIVE";
        break;
    case SHARED:
        stateName = "SHARED";
        break;
    case INVALID:
        stateName = "INVALID";
        break;
    }

    return stateName;
}

void Cache::setState(uint32_t address, MesiState state)
{
    uint32_t index;
    try
    {
        index = this->findBlock(address);
    }
    catch (AddressNotFound &err) // State setting associated with miss
    {
        // Find out what block will be evicted and set state preemptively
        index = this->blockToEvict(address);
    }

    // Keep track of previously set state
    MesiState lastState = this->mesiStates.at(index);
    this->previousMesiStates.at(index) = lastState;

    DEBUG << str(this) << " setting state for address " << str(address) << " to " << stateName(state) << " from " << stateName(lastState) << "\n";
    this->mesiStates.at(index) = state;

    if (this->mesiStates.at(index) == INVALID)
    {
        this->valid.at(index) = false;
    }
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
}
