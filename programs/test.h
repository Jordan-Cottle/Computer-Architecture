#include <assert.h>

#include "pipeline.h"
#include "simulation.h"

#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "store.h"

Fetch fetchUnit = Fetch(&Simulation::cpu);
Decode decodeUnit = Decode(&Simulation::cpu);
Execute executeUnit = Execute(&Simulation::cpu);
StorePipeline storeUnit = StorePipeline(&Simulation::cpu);

constexpr float PI = 3.141592654f;
constexpr float E = 2.718281828f;

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