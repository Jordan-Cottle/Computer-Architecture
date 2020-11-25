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

void load_binary(std::string fileName, MemoryInterface *memory, uint32_t startAddress)
{

    std::ifstream dataFile(fileName, std::ios::binary);

    if (!dataFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint8_t byte_count = 0;
    uint32_t word = 0;
    uint8_t data;
    uint32_t memAddress = startAddress;
    while (dataFile.read((char *)&data, sizeof(data)))
    {
        if (byte_count == 4)
        {
            memory->write(memAddress, word);
            memAddress += 4;
            word = 0;
            byte_count = 0;
        }

        word |= data << (byte_count * 8);
        byte_count += 1;
    }

    if (byte_count != 0)
    {
        while (byte_count != 4)
        {
            word = (word << 8);
            byte_count += 1;
        }
        memory->write(memAddress, word);
    }
}