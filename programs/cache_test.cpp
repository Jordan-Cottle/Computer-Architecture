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

    assert(masterEventQueue.size() == 1);
    Event *nextEvent = masterEventQueue.top();
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == (ulong)cache->accessTime);
    assert(nextEvent->device == cache);

    simulationClock.cycle = nextEvent->time;
    masterEventQueue.pop();
    nextEvent->device->process(nextEvent);

    assert(masterEventQueue.size() == 1);
    nextEvent = masterEventQueue.top();
    assert(nextEvent->type == "MemoryReady");         // Same event type
    assert(nextEvent->time == simulationClock.cycle); // Scheduled for same time
    assert(nextEvent->device == &testPipeline);       // Passed back to requesting device
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
            assert(cache->cacheAddress(address) == i);
        }
    }
}

void testMemoryAccess()
{
    // Seed cache with data
    for (uint32_t i = 0; i < CACHE_SIZE; i += 4)
    {
        cache->write(i, (int)(CACHE_SIZE - i));
    }

    for (uint32_t i = 0; i < CACHE_SIZE; i += 4)
    {
        assert(cache->readInt(i) == (int)(CACHE_SIZE - i));
    }
}

void testReplacementPolicy()
{
}

int main()
{
    testEventHandling();
    testAdressing();
    testMemoryAccess();
    testReplacementPolicy();
}