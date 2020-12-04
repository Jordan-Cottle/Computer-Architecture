#include "test.h"

#include "cache.h"

using namespace Simulation;

constexpr uint32_t CACHE_SIZE = 128;
constexpr uint32_t CACHE_DELAY = 3;
constexpr uint32_t BLOCK_SIZE = 16;
constexpr uint32_t ASSOCIATIVITY = DIRECT_MAPPED;

Memory *memory = new Memory(CACHE_DELAY * 10, CACHE_SIZE * 4);
MemoryBus *memBus = new MemoryBus(BUS_ARBITRATION_TIME, memory);
std::vector<int> mockData = std::vector<int>(memory->size / 4);

void testEventHandling()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);
    cache->request(0, &testPipeline);

    // Compulsory miss, cache reads from memory through memBus
    assert(masterEventQueue.size() == 1);
    Event *nextEvent = masterEventQueue.pop();
    std::cout << nextEvent << "\n";
    assert(nextEvent->type == "ProcessRequest");
    assert(nextEvent->time == (ulong)memBus->accessTime); // Arbitration delay
    assert(nextEvent->device == memBus);

    simulationClock.cycle = nextEvent->time;
    nextEvent->device->process(nextEvent);

    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.pop();
    std::cout << nextEvent << "\n";
    assert(nextEvent->type == "MemoryReadReady");
    assert(nextEvent->time == (ulong)memory->accessTime + memBus->accessTime); // Long delay for cache miss
    assert(nextEvent->device == cache);

    simulationClock.cycle = nextEvent->time;
    nextEvent->device->process(nextEvent);

    // Internal cache reading from its own memory for a read hit
    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReadReady");                                           // Same event type
    assert(nextEvent->time == memory->accessTime + cache->accessTime + memBus->accessTime); // Scheduled after cache access time
    assert(nextEvent->device == cache);                                                     // Passed back to cache (this is the actual cache read)

    simulationClock.cycle = nextEvent->time; // Update sim time to event time
    nextEvent->device->process(nextEvent);

    // Test pipeline now gets its message after memDelay + cacheDelay
    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReadReady");     // Same event type
    assert(nextEvent->time == simulationClock.cycle); // Scheduled for same time as cache internal read
    assert(nextEvent->device == &testPipeline);       // Passed back to original requesting device

    nextEvent->device->process(nextEvent);

    delete cache;
}

void testAdressing()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);

    for (uint32_t i = 0; i < CACHE_SIZE; i++)
    {
        assert(cache->offset(i) == i % BLOCK_SIZE);
        assert(cache->index(i) == i / BLOCK_SIZE);
        for (uint32_t j = 0; j < memory->size / BLOCK_SIZE; j++)
        {
            uint32_t address = i + (CACHE_SIZE * j);
            assert(cache->tag(address) == j);
        }
    }

    delete cache;
}

void testBlockLoad()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);
    // Ensure cache is blank to start
    for (uint32_t i = 0; i < CACHE_SIZE; i += 4)
    {
        assert(cache->data->readInt(i) == 0);
        assert(cache->tags[cache->index(i)] == 0);
        assert(cache->valid[cache->index(i)] == false);
    }

    for (uint32_t i = 0; i < CACHE_SIZE / BLOCK_SIZE; i++)
    {
        uint32_t start = i * BLOCK_SIZE;
        cache->loadBlock(start);

        for (uint32_t j = start; j < start + BLOCK_SIZE; j += sizeof(int))
        {
            assert(cache->data->readInt(j) == memory->readInt(j));
            assert(cache->readInt(j) == memory->readInt(j));
        }

        for (uint32_t j = start + BLOCK_SIZE; j < CACHE_SIZE; j += 4)
        {
            assert(cache->data->readInt(j) == 0);
            assert(cache->tags[cache->index(j)] == 0);
            assert(cache->valid[cache->index(j)] == false);
        }
    }

    delete cache;
}

void processRequest(Cache *cache, uint32_t address, bool read = true, SimulationDevice *requestor = &testPipeline)
{
    cache->request(address, requestor, read);
    assert(masterEventQueue.size() == 1);

    // Cycle through entire event chain for request
    while (!masterEventQueue.empty())
    {
        simulationClock.cycle = masterEventQueue.top()->time;
        masterEventQueue.tick(simulationClock.cycle);
    }
}

void testMemoryAccess()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);
    // Fill up cache
    for (uint32_t i = 0; i < cache->size; i += 4)
    {
        processRequest(cache, i);
        int data = -int(i);
        cache->write(i, (void *)&data, sizeof(data));
    }

    // Assert that every block has valid data, cache fully saturated
    for (uint32_t i = 0; i < cache->size; i += 4)
    {
        assert(cache->valid[i / cache->blockSize]);
        // Check right data was written
        assert(cache->readInt(i) == -int(i));
    }

    // Cause a ton of conflicts
    for (uint32_t i = 0; i < cache->size; i += 4)
    {
        uint32_t outOfCacheAddress = i + cache->size;
        processRequest(cache, outOfCacheAddress);
        cache->write(outOfCacheAddress, (void *)&outOfCacheAddress, sizeof(outOfCacheAddress));

        // Assert that value was updated even though cache was full
        assert(cache->readUint(outOfCacheAddress) == outOfCacheAddress);
    }

    delete cache;
}

void testReplacementPolicy()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, 4, memBus);

    // All bits start off unset
    for (auto bit : cache->lruBits)
    {
        assert(!bit);
    }

    processRequest(cache, 0);
    cache->readInt(0);
    // First block set
    assert(cache->lruBits[0]);
    // Next block not
    assert(!cache->lruBits[1]);

    // Another read should leave state the same
    cache->readInt(0);
    assert(cache->lruBits[0]);
    assert(!cache->lruBits[1]);

    // Wrap around request to hit same block again
    assert(!cache->valid[1]);
    processRequest(cache, cache->size);
    assert(cache->valid[1]);

    cache->readInt(cache->size);
    assert(cache->lruBits[0]);

    // Next invalid block filled in
    assert(cache->lruBits[1]);

    // Fill up set
    for (uint32_t i = 2; i < cache->associativity; i++)
    {
        uint32_t memoryAddress = (i)*cache->size;
        assert(!cache->valid[i]);
        assert(!cache->lruBits[i]);
        processRequest(cache, memoryAddress);
        cache->readInt(memoryAddress);
        assert(cache->valid[i]);

        // Other bits still set
        assert(cache->lruBits[i - 1]);
        // New bit set
        assert(cache->lruBits[i]);
    }

    // All bits are set as recently used, make a request to one to set just it
    cache->readInt(0);
    assert(cache->lruBits[0]);
    // Others unset
    assert(!cache->lruBits[1]);
    assert(!cache->lruBits[2]);
    assert(!cache->lruBits[3]);

    // First unused block should be evicted
    uint32_t evicted = cache->blockToEvict(cache->size * cache->associativity);
    assert(evicted == 1);

    // Read index 1 value twice to make it the priority keep
    cache->readInt(cache->size);
    cache->readInt(cache->size);

    assert(cache->lruBits[1]);
    // Others unset
    assert(!cache->lruBits[0]);
    assert(!cache->lruBits[2]);
    assert(!cache->lruBits[3]);

    // Use uncached address to validate eviction target
    evicted = cache->blockToEvict(cache->size * cache->associativity);
    assert(evicted == 0);

    delete cache;
}

void testMesiStateChange()
{
    Cache *local = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);
    Cache *other = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);

    uint32_t address = 0;
    local->loadBlock(address); // Make sure cache thinks the memory is valid
    uint32_t block = local->findBlock(address);

    assert(local->mesiStates[block] == INVALID);
    assert(other->mesiStates[block] == INVALID);

    // Test receiving invalidate events
    MesiEvent *mesiEvent = new MesiEvent(INVALIDATE, address, other, false);
    bool found = local->snoop(mesiEvent);
    assert(!found);
    assert(local->mesiStates[block] == INVALID);
    assert(other->mesiStates[block] == INVALID);

    local->mesiStates[block] = MODIFIED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    local->mesiStates[block] = EXCLUSIVE;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    local->mesiStates[block] = SHARED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    // Test receiving mem read events
    mesiEvent->signal = MEM_READ;
    mesiEvent->read = true;
    local->mesiStates[block] = MODIFIED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == SHARED);

    local->mesiStates[block] = EXCLUSIVE;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == SHARED);

    local->mesiStates[block] = SHARED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == SHARED);

    local->mesiStates[block] = INVALID;
    local->valid.at(block) = false;
    found = local->snoop(mesiEvent);
    assert(!found);
    assert(local->mesiStates[block] == INVALID);

    // Test receiving rwitm events
    mesiEvent->signal = RWITM;
    mesiEvent->read = true;
    local->mesiStates[block] = MODIFIED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    local->mesiStates[block] = EXCLUSIVE;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    local->mesiStates[block] = SHARED;
    local->valid.at(block) = true;
    found = local->snoop(mesiEvent);
    assert(found);
    assert(local->mesiStates[block] == INVALID);

    local->mesiStates[block] = INVALID;
    found = local->snoop(mesiEvent);
    assert(!found);
    assert(local->mesiStates[block] == INVALID);
    assert(other->mesiStates[block] == INVALID);
}

void testMesiSignalGeneration()
{
    memBus->caches.clear();
    Cache *local = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);
    Cache *other = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memBus);

    // Assert caches are registered with membus
    assert(memBus->caches.size() == 2);
    assert(memBus->caches.at(0) == local);
    assert(memBus->caches.at(1) == other);

    uint32_t address = 0;
    uint32_t index = local->index(address);

    // Both caches start in invalid
    assert(local->mesiStates[index] == INVALID);
    assert(other->mesiStates[index] == INVALID);

    // Local cache reads an address I -> E
    processRequest(local, address);
    assert(local->mesiStates[index] == EXCLUSIVE);
    assert(other->mesiStates[index] == INVALID);
    local->readUint(address);

    // Other cache reads same address E -> S (I -> S)
    processRequest(other, address);
    assert(local->mesiStates[index] == SHARED);
    assert(other->mesiStates[index] == SHARED);
    other->readUint(address);
}

void seedMemory()
{
    // Seed memory with test data
    for (uint32_t i = 0; i < memory->size / 4; i++)
    {
        int datum = i;
        mockData[i] = datum;
        uint32_t memAddress = i * sizeof(int);
        memory->write(memAddress, (void *)&datum, sizeof(datum));
    }
}

int main()
{
    seedMemory();

    std::cout << "\nTesting cache event handling system\n";
    testEventHandling();

    std::cout << "\nTesting cache address processing system\n";
    testAdressing();

    std::cout << "\nTesting block loading procedure\n";
    testBlockLoad();

    std::cout << "\nTesting cache memory addressing system\n";
    testMemoryAccess();

    std::cout << "\nTesting cache replacement policy\n";
    testReplacementPolicy();

    std::cout << "\nTesting mesi protocol\n";
    testMesiStateChange();
    testMesiSignalGeneration();

    return 0;
}