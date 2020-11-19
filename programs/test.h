#include <assert.h>
#include <random>

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