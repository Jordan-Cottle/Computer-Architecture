#include <fstream>
#include <math.h>

#include "cache.h"

#include "test.h"
using namespace Simulation;

std::vector<uint32_t> loadTrace(std::string fileName)
{

    std::vector<uint32_t> accessAddresses = std::vector<uint32_t>();
    std::ifstream traceFile(fileName, std::ios::binary);

    if (!traceFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint32_t address;
    while (traceFile.read((char *)&address, sizeof(uint32_t)))
    {
        accessAddresses.push_back(address);
    }

    return accessAddresses;
}

struct CacheResult : printable
{
    uint32_t accesses;
    uint32_t hits;
    uint32_t compulsoryMisses;
    uint32_t totalTime;

    Cache *cache;

    CacheResult(Cache *cache)
    {
        this->accesses = cache->accesses;
        this->hits = cache->hits;
        this->compulsoryMisses = cache->compulsoryMisses;

        this->cache = cache;
        this->totalTime = simulationClock.cycle;
    }

    float compulsoryMissRate()
    {
        return this->compulsoryMisses / float(this->accesses);
    }

    float hitRate()
    {
        return this->hits / float(this->accesses);
    }

    std::string __str__()
    {
        return "Cache Report:\n\tHit Rate = " + str(this->hitRate()) + "\n\t" +
               "Compulsory Miss Rate = " + str(this->compulsoryMissRate()) + "\n\t" +
               "Total sim ticks elapsed = " + str(this->totalTime) + "\n\t" +
               "Cache hit time = " + str(this->cache->accessTime);
    }
};

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

CacheResult *runSimulation(Cache *cache, std::vector<uint32_t> trace)
{
    simulationClock.cycle = 0;
    for (auto address : trace)
    {
        processRequest(cache, address);

        uint32_t value = cache->readUint(address);

        std::cout << "Cached value for memory address " << address << ": " << value << "\n";

        uint32_t offset = address % 4;
        uint32_t alignedAddress = address - offset;
        // Check validity of data at aligned addresses
        assert(cache->readUint(alignedAddress) == alignedAddress);
        // Guessing/computing the MSBs of the next word isn't worth validating here
    }

    return new CacheResult(cache);
}

int main()
{
    std::vector<uint32_t> trace = loadTrace("memory_trace");
    std::vector<CacheResult *> results = std::vector<CacheResult *>();

    // Create and write fake data to memory
    Memory *memory = new Memory(100, 1024 * 8);
    for (uint32_t i = 0; i < memory->size; i += 4)
    {
        memory->write(i, i);
    }

    /*
    Simulation 1:
    Cache Size: 256B
    Cache Line Size: 32B
    Associativity: None
    */
    uint32_t cacheSize = 256;
    uint32_t blockSize = 32;
    uint32_t associativity = DIRECT_MAPPED;
    uint32_t accessTime = uint32_t(ceil(log2(cacheSize / float(blockSize)))) * associativity;

    Cache *cache = new Cache(accessTime, cacheSize, blockSize, associativity, memory);
    results.push_back(runSimulation(cache, trace));

    /*
    Simulation 2:
    Cache Size: 512B
    Cache Line Size: 32B
    Associativity: None
    */
    cacheSize = 512;
    blockSize = 32;
    associativity = DIRECT_MAPPED;
    accessTime = uint32_t(ceil(log2(cacheSize / float(blockSize)))) * associativity;

    cache = new Cache(accessTime, cacheSize, blockSize, associativity, memory);
    results.push_back(runSimulation(cache, trace));

    /*
    Simulation 3:
    Cache Size: 256B
    Cache Line Size: 64B
    Associativity: None
    */
    cacheSize = 256;
    blockSize = 64;
    associativity = DIRECT_MAPPED;
    accessTime = uint32_t(ceil(log2(cacheSize / float(blockSize)))) * associativity;

    cache = new Cache(accessTime, cacheSize, blockSize, associativity, memory);
    results.push_back(runSimulation(cache, trace));

    /*
    Simulation 4:
    Cache Size: 256B
    Cache Line Size: 32B
    Associativity: 4-Way
    */
    cacheSize = 256;
    blockSize = 32;
    associativity = 4;
    accessTime = uint32_t(ceil(log2(cacheSize / float(blockSize)))) * associativity;

    cache = new Cache(accessTime, cacheSize, blockSize, associativity, memory);
    results.push_back(runSimulation(cache, trace));

    std::cout << "\n~~~~~~~~~~~~~~Cache trace simulation results~~~~~~~~~~~~~~\n";
    int i = 0;
    for (auto result : results)
    {
        std::cout << "Simulation " << ++i << " " << result << "\n";
    }
}
