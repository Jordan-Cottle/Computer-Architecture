#include "test.h"

#include "cache.h"

using namespace Simulation;

constexpr uint32_t CACHE_SIZE = 512;
constexpr uint32_t CACHE_DELAY = 3;
constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t ASSOCIATIVITY = DIRECT_MAPPED;

Memory *memory = new Memory(CACHE_DELAY * 10, CACHE_SIZE * 32);
std::vector<int> mockData = std::vector<int>(CACHE_SIZE / 4);

void testEventHandling()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
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
    delete cache;
}

void testAdressing()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);
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
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);

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
            std::cout << "Memory address " << j << ": ";
            std::cout << mockData[j / 4] << " == ";
            std::cout << memory->readInt(j) << "\n";
            std::cout << "Cache data: " << cache->data->readInt(j) << "\n";
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
    while (!masterEventQueue.empty())
    {
        simulationClock.cycle = masterEventQueue.top()->time;
        masterEventQueue.tick(simulationClock.cycle);
    }
}

void testMemoryAccess()
{
    Cache *cache = new Cache(CACHE_DELAY, CACHE_SIZE, BLOCK_SIZE, DIRECT_MAPPED, memory);

    // Load values into cache
    for (uint32_t i = 0; i < CACHE_SIZE / 4; i++)
    {
        uint32_t memAddress = i * sizeof(int);
        processRequest(cache, memAddress);
        cache->write(memAddress, mockData[i]);
        assert(cache->data->readInt(memAddress) == mockData[i]);
    }

    for (uint32_t i = 0; i < CACHE_SIZE / 4; i++)
    {
        uint32_t memAddress = i * sizeof(int);
        assert(cache->data->readInt(memAddress) == mockData[i]);
    }

    // Check that values can be read back
    for (uint32_t i = 0; i < CACHE_SIZE / 4; i++)
    {
        uint32_t memAddress = i * sizeof(int);
        assert(cache->readInt(memAddress) == mockData[i]);
    }

    // TODO: test memory accesses for tag != 0
    delete cache;
}

void testReplacementPolicy()
{
    // TODO specific tests for verifying associativity and replacement policy
}

void seedMemory()
{
    // Seed memory with test data
    for (uint32_t i = 0; i < CACHE_SIZE / 4; i++)
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

    std::cout << "Testing cache event handling system\n";
    testEventHandling();
    std::cout << "Testing cache address processing system\n";
    testAdressing();
    std::cout << "Testing block loading procedure\n";
    testBlockLoad();
    std::cout << "Testing cache memory addressing system\n";
    testMemoryAccess();
    std::cout << "Testing cache replacement policy\n";
    testReplacementPolicy();
}