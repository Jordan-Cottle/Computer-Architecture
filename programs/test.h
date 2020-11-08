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
};

TestPipeline testPipeline;