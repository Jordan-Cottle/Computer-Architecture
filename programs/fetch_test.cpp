#include "test.h"

#include "fetch.h"

using namespace Simulation;

int main()
{
    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");
    RawInstruction *instruction = new RawInstruction(cpu.memory->readUint(0));

    assert(fetchUnit.staged() == NULL);

    fetchUnit.tick();

    // Let memory request and work events process through the unit
    processEvents();

    assert(testPipeline.staged() != NULL);
    assert(testPipeline.staged()->data == instruction->data);

    testPipeline.tick();
}