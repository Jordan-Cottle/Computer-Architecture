#include <math.h>
#include <string>
#include <fstream>

#include "test.h"
using namespace Simulation;

#include "cache.h"
#include "memory_router.h"

struct CacheConfig : printable
{
    uint32_t size;
    uint32_t lineSize;
    uint32_t associativity;
    uint32_t accessTime;

    CacheConfig(uint32_t size, uint32_t lineSize, uint32_t associativity)
    {
        this->size = size;
        this->lineSize = lineSize;
        this->associativity = associativity;
        this->accessTime = uint32_t(ceil(log2(size / float(lineSize)))) * associativity;
    }

    std::string __str__()
    {
        return "Cache Config:\n\tSize: " + str(this->size) + "\n\t" +
               "Line Size: " + str(this->lineSize) + "\n\t" +
               "Associativity: " + str(this->associativity) + "\n\t" +
               "Access Time: " + str(this->accessTime);
    }
};

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
        return "Cache:\n\tHit Rate = " + str(this->hitRate()) + "\n\t" +
               "Compulsory Miss Rate = " + str(this->compulsoryMissRate()) + "\n\t" +
               "Cache hit time = " + str(this->cache->accessTime);
    }

    std::string json()
    {
        return "{'hit_rate':" + str(this->hitRate()) + "," +
               "'compulsory_miss_rate':" + str(this->compulsoryMissRate()) + "," +
               "'cache_access_time':" + str(this->cache->accessTime) + "," +
               "'cache_size':" + str(this->cache->size) + "," +
               "'cache_block_size':" + str(this->cache->blockSize) + "," +
               "'cache_associativity':" + str(this->cache->associativity) + "}";
    }

    bool operator<(const CacheResult &other)
    {
        return this->totalTime < other.totalTime;
    }
};

struct SimulationResult : printable
{
    float cpi0;
    CacheResult *i0;
    CacheResult *d0;
    uint32_t cpu0Clocks;

    float cpi1;
    CacheResult *i1;
    CacheResult *d1;
    uint32_t cpu1Clocks;

    float averageCpi;
    ulong timeElapsed;

    SimulationResult(Cpu *cpu0, Cpu *cpu1)
    {
        this->cpi0 = cpu0->cpi();
        this->cpu0Clocks = cpu0->clocksProcessed;
        this->cpi1 = cpu1->cpi();
        this->cpu1Clocks = cpu1->clocksProcessed;
        this->averageCpi = (this->cpi0 + this->cpi1) / 2;
        this->timeElapsed = simulationClock.cycle;

        MemoryRouter *router = (MemoryRouter *)cpu0->memory;
        this->i0 = new CacheResult((Cache *)router->instrutionCache);
        this->d0 = new CacheResult((Cache *)router->dataCache);

        router = (MemoryRouter *)cpu1->memory;
        this->i1 = new CacheResult((Cache *)router->instrutionCache);
        this->d1 = new CacheResult((Cache *)router->dataCache);
    }

    bool operator<(const SimulationResult &other)
    {
        return this->averageCpi < other.averageCpi;
    }

    std::string __str__()
    {
        return "Simulation result:\n\tSimTicks elapsed: " + str(this->timeElapsed) + "\n\t" +
               "Average cpi: " + str(this->averageCpi) + "\n\t" +
               "Cpu 0 :\n\t\t" +
               "Clocks: " + str(this->cpu0Clocks) + "\n\t\t" +
               "Cpi: " + str(this->cpi0) + "\n\t\t" +
               "Instruction " + addIndent(str(this->i0), 2) + "\n\t\t" +
               "Data " + addIndent(str(this->d0), 2) + "\n\t" +
               "Cpu 1:\n\t\t" +
               "Clocks: " + str(this->cpu1Clocks) + "\n\t\t" +
               "Cpi: " + str(this->cpi1) + "\n\t\t" +
               "Instruction " + addIndent(str(this->i1), 2) + "\n\t\t" +
               "Data " + addIndent(str(this->d1), 2);
    }

    std::string json()
    {
        return "{'time_elapsed':" + str(this->timeElapsed) + "," +
               "'overall_cpi':" + str(this->averageCpi) + "," +
               "'cpu0': {" +
               "'cpi':" + str(this->cpi0) + "," +
               "'instruction_cache':" + addIndent(this->i0->json(), 2) + "," +
               "'data_cache':" + addIndent(this->d0->json(), 2) + "}," +
               "'cpu1': {" +
               "'cpi':" + str(this->cpi1) + "," +
               "'instruction_cache':" + addIndent(this->i1->json(), 2) + "," +
               "'data_cache':" + addIndent(this->d1->json(), 2) + "}}";
    }
};

Cpu *constructCpu(MemoryBus *memBus, CacheConfig iCacheConfig, CacheConfig dCacheConfig)
{
    Cache *instructionCache = new Cache(iCacheConfig.accessTime, iCacheConfig.size, iCacheConfig.lineSize, iCacheConfig.associativity, memBus);
    Cache *dataCache = new Cache(dCacheConfig.accessTime, dCacheConfig.size, dCacheConfig.lineSize, dCacheConfig.associativity, memBus);

    MemoryRouter *router = new MemoryRouter(instructionCache, dataCache);

    Cpu *cpu = new Cpu(router);

    cpu->addPipeline(new Fetch(cpu))
        ->addPipeline(new Decode(cpu))
        ->addPipeline(new Execute(cpu))
        ->addPipeline(new StorePipeline(cpu));

    return cpu;
}

SimulationResult runSimulation(CacheConfig iConfig, CacheConfig dConfig)
{
    const int ARRAY_A_START = 0x400;
    const int ARRAY_B_START = 0x800;
    const int ARRAY_C_START = 0xC00;
    const int ARRAY_D_START = 0x1000;
    const int ARRAY_SIZE = (ARRAY_B_START - ARRAY_A_START) / sizeof(float);

    const int STACK0_START = 0x2ff;
    const int STACK1_START = 0x3ff;

    // Arrays for implementing/testing CPU0.s
    float ARRAY_A[ARRAY_SIZE];
    float ARRAY_B[ARRAY_SIZE];
    float ARRAY_C[ARRAY_SIZE];
    float ARRAY_D[ARRAY_SIZE];

    Memory *ram = new Memory(100, MEMORY_SIZE, {0x100, 0x200, 0x1400});
    MemoryBus *memBus = new MemoryBus(BUS_ARBITRATION_TIME, ram);

    Cpu *cpu0 = constructCpu(memBus, iConfig, dConfig);
    Cpu *cpu1 = constructCpu(memBus, iConfig, dConfig);

    // Initialize arrays in fp memory
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = get_random();
        float b = get_random();
        ram->write(ARRAY_A_START + memOffset, a);
        ram->write(ARRAY_B_START + memOffset, b);

        // Initialize arrays for implementing/testing CPU0.s
        ARRAY_A[i] = a;
        ARRAY_B[i] = b;
    }

    cpu0->loadProgram("CPU0.bin", 0, ram);
    cpu1->loadProgram("CPU1.bin", 0x100, ram);

    cpu0->intRegister.write(14, STACK0_START); // Set stack pointer at bottom of stack
    cpu1->intRegister.write(14, STACK1_START); // Set stack pointer at bottom of stack

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, cpu0));
    masterEventQueue.push(new Event("Tick", 0, cpu1));

    while ((!cpu0->complete || !cpu1->complete))
    {
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        // float a = ram->readFloat(ARRAY_A_START + memOffset);
        // float b = ram->readFloat(ARRAY_B_START + memOffset);
        float c = ram->readFloat(ARRAY_C_START + memOffset);
        float d = ram->readFloat(ARRAY_D_START + memOffset);

        // CPU0.s functionality
        ARRAY_C[i] = ARRAY_A[i] + ARRAY_B[i];
        // CPU1.s functionality
        ARRAY_D[i] = ARRAY_A[i] - ARRAY_B[i];

        // Ensure simulation and implementation match
        assert(ARRAY_C[i] == c);
        assert(ARRAY_D[i] == d);
    }

    return SimulationResult(cpu0, cpu1);
}

int main(int argc, char *argv[])
{
    uint32_t iCacheSize = std::stoi(argv[1]);
    uint32_t dCacheSize = std::stoi(argv[2]);
    uint32_t lineSize = std::stoi(argv[3]);
    uint32_t iAssociativity = std::stoi(argv[4]);
    uint32_t dAssociativity = std::stoi(argv[5]);

    CacheConfig iConfig = CacheConfig(iCacheSize, lineSize, iAssociativity);
    std::cout << "Instruction " << iConfig << "\n";
    CacheConfig dConfig = CacheConfig(dCacheSize, lineSize, dAssociativity);
    std::cout << "Data " << dConfig << "\n\n\n";

    SimulationResult result = runSimulation(iConfig, dConfig);
    std::cout << result << "\n";

    std::ofstream outFile;

    outFile.open(argv[6]);

    outFile << findAndReplaceAll(result.json(), "'", "\"") << "\n";
}
