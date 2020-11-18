#include <fstream>

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

    Cache *cache;

    CacheResult(Cache *cache)
    {
        this->accesses = cache->accesses;
        this->hits = cache->hits;
        this->compulsoryMisses = cache->compulsoryMisses;

        this->cache = cache;
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
               "Compulsory Miss Rate = " + str(this->compulsoryMissRate());
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

    constexpr ulong ACCESS_TIME = 1;

    // Create and write fake data to memory
    Memory *memory = new Memory(10, 1024 * 8);
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

    Cache *cache = new Cache(ACCESS_TIME, cacheSize, blockSize, associativity, memory);
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

    cache = new Cache(ACCESS_TIME, cacheSize, blockSize, associativity, memory);
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

    cache = new Cache(ACCESS_TIME, cacheSize, blockSize, associativity, memory);
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

    cache = new Cache(ACCESS_TIME, cacheSize, blockSize, associativity, memory);
    results.push_back(runSimulation(cache, trace));

    std::cout << "\n~~~~~~~~~~~~~~Cache trace simulation results~~~~~~~~~~~~~~\n";
    int i = 0;
    for (auto result : results)
    {
        std::cout << "Simulation " << ++i << " " << result << "\n";
    }
}
