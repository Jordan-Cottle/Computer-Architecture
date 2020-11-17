#include "test.h"

#include "cache.h"

using namespace Simulation;

constexpr uint32_t CACHE_SIZE = 128;
constexpr uint32_t CACHE_DELAY = 3;
constexpr uint32_t BLOCK_SIZE = 16;
constexpr uint32_t ASSOCIATIVITY = DIRECT_MAPPED;

Memory *memory = new Memory(CACHE_DELAY * 10, CACHE_SIZE * 4);
std::vector<int> mockData = std::vector<int>(memory->size / 4);

void testEventHandling(Cache *cache)
{
    cache->request(0, &testPipeline);

    // Compulsory miss, cache reads from main memory
    assert(masterEventQueue.size() == 1);
    Event *nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == (ulong)memory->accessTime); // Long delay for cache miss
    assert(nextEvent->device == cache);

    simulationClock.cycle = nextEvent->time;
    nextEvent->device->process(nextEvent);

    // Internal cache reading from its own memory for a read hit
    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReady");                          // Same event type
    assert(nextEvent->time == memory->accessTime + cache->accessTime); // Scheduled after cache access time
    assert(nextEvent->device == cache);                                // Passed back to cache (this is the actual cache read)

    simulationClock.cycle = nextEvent->time; // Update sim time to event time
    nextEvent->device->process(nextEvent);

    // Test pipeline now gets its message after memDelay + cacheDelay
    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReady");         // Same event type
    assert(nextEvent->time == simulationClock.cycle); // Scheduled for same time as cache internal read
    assert(nextEvent->device == &testPipeline);       // Passed back to original requesting device

    nextEvent->device->process(nextEvent);
}

void testAdressing(Cache *cache)
{
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
}

void testBlockLoad(Cache *cache)
{
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
}

void processRequest(Cache *cache, uint32_t address)
{
    cache->request(address, &testPipeline);
    assert(masterEventQueue.size() == 1);

    // Cycle through entire event chain for request
    while (!masterEventQueue.empty())
    {
        simulationClock.cycle = masterEventQueue.top()->time;
        masterEventQueue.tick(simulationClock.cycle);
    }
}

void testMemoryAccess(Cache *cache)
{
    // Read all values from memory through the cache
    for (uint32_t i = 0; i < memory->size / 4; i++)
    {
        uint32_t memAddress = i * sizeof(int);
        processRequest(cache, memAddress);
        assert(cache->readInt(memAddress) == mockData[i]);
    }
}

void testReplacementPolicy(Cache *cache)
{
    // Fill up cache
    for (uint32_t i = 0; i < cache->size; i += 4)
    {
        processRequest(cache, i);
        cache->write(i, -int(i));
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
        cache->write(outOfCacheAddress, outOfCacheAddress);

        // Assert that value was updated even though cache was full
        assert(cache->readUint(outOfCacheAddress) == outOfCacheAddress);
    }
}

void seedMemory()
{
    // Seed memory with test data
    for (uint32_t i = 0; i < memory->size / 4; i++)
    {
        int datum = i;
        mockData[i] = datum;
        uint32_t memAddress = i * sizeof(int);
        memory->write(memAddress, datum);
    }
}

int main()
{
    seedMemory();
    Cache *cache;

    std::cout << "\nTesting cache event handling system\n";
    cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
    testEventHandling(cache);
    delete cache;

    std::cout << "\nTesting cache address processing system\n";
    cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
    testAdressing(cache);
    delete cache;

    std::cout << "\nTesting block loading procedure\n";
    cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
    testBlockLoad(cache);
    delete cache;

    std::cout << "\nTesting cache memory addressing system\n";
    cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
    testMemoryAccess(cache);
    delete cache;

    std::cout << "\nTesting cache replacement policy\n";
    cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, 4, memory);
    testReplacementPolicy(cache);
    delete cache;

    return 0;
}