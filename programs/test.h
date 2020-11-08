#include <assert.h>

#include "pipeline.h"
#include "simulation.h"

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