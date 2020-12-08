#include <assert.h>
#include <random>
#include <fstream>

#include "binary.h"

#include "pipeline.h"
#include "simulation.h"

#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "store.h"

#include "memory_router.h"

constexpr float PI = 3.141592654f;
constexpr float E = 2.718281828f;

float get_random()
{
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(-100, 100);
    return dis(e);
}

// Echo any events/instructions for debugging partial pipelines
struct TestPipeline : Pipeline
{
    Event *lastEvent;
    TestPipeline() : Pipeline("TestPipeline")
    {
    }

    void tick()
    {
        RawInstruction *staged = this->staged();

        std::cout << this->type << " T " << Simulation::simulationClock.cycle << ": ";
        if (staged == NULL)
        {
            std::cout << " no instruction\n";
        }
        else
        {
            std::cout << this->staged() << "\n";
        }

        Pipeline::tick();
    }

    void process(Event *event)
    {
        std::cout << "Event received: " << str(event) << "\n";

        this->lastEvent = event;
    }
};

TestPipeline testPipeline;

Cpu cpu;
Fetch fetchUnit = Fetch(&cpu);
Decode decodeUnit = Decode(&cpu);
Execute executeUnit = Execute(&cpu);
StorePipeline storeUnit = StorePipeline(&cpu);

void processEvents()
{
    while (!Simulation::masterEventQueue.empty())
    {
        Simulation::simulationClock.cycle = Simulation::masterEventQueue.top()->time;
        Simulation::masterEventQueue.tick(Simulation::simulationClock.cycle);
    }
}

void load_binary(std::string fileName, Memory *memory, uint32_t startAddress)
{

    std::ifstream dataFile(fileName, std::ios::binary);

    if (!dataFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint8_t data;
    uint32_t memAddress = startAddress;
    while (dataFile.read((char *)&data, sizeof(data)))
    {
        memory->data[memAddress++] = data;
    }
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
        this->totalTime = Simulation::simulationClock.cycle;
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
        this->timeElapsed = Simulation::simulationClock.cycle;

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