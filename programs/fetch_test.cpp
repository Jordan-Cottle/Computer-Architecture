#include "test.h"

#include "fetch.h"

using namespace Simulation;

int main()
{
    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    assert(fetchUnit.staged() == NULL);

    fetchUnit.tick();
    assert(testPipeline.staged() != NULL);
    RawInstruction *instruction = fetchUnit.staged();
    assert(testPipeline.staged() == instruction);

    testPipeline.tick();
}