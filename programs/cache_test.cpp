#include "test.h"

#include "cache.h"

using namespace Simulation;

constexpr uint32_t CACHE_SIZE = 512;
constexpr uint32_t CACHE_DELAY = 3;
constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t ASSOCIATIVITY = DIRECT_MAPPED;

Memory *memory = new Memory(CACHE_DELAY * 10, CACHE_SIZE * 32);
Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, ASSOCIATIVITY, memory);

void testEventHandling()
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

void testAdressing()
{
    for (uint32_t i = 0; i < CACHE_SIZE; i++)
    {
        assert(cache->blockOffset(i) == i % BLOCK_SIZE);
        assert(cache->blockIndex(i) == i / BLOCK_SIZE);
        for (uint32_t j = 0; j < memory->size / CACHE_SIZE; j++)
        {
            uint32_t address = i + (CACHE_SIZE * j);
            assert(cache->tag(address) == j);
        }
    }
}

void testMemoryAccess()
{
    // Seed cache with data
    std::vector<int> data = std::vector<int>(CACHE_SIZE / 4);

    for (uint32_t i = 0; i < CACHE_SIZE; i += 4)
    {
        int datum = rand();
        data[i / 4] = datum;
        cache->write(i, datum);
    }

    for (uint32_t i = 0; i < CACHE_SIZE; i += 4)
    {
        assert(cache->readInt(i) == data[i / 4]);
    }

    // TODO: test memory accesses for tag != 0
}

void testReplacementPolicy()
{
    // TODO specific tests for verifying and associativity replacement policy
}

int main()
{
    testEventHandling();
    testAdressing();
    testMemoryAccess();
    testReplacementPolicy();
}