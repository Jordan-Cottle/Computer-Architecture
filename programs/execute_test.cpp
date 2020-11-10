#include "test.h"
using namespace Simulation;

#include "arithmetic_instruction.h"

int main()
{
    cpu.addPipeline(&executeUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.memory->readUint(4));

    Add *add = new Add(instruction);

    cpu.intRegister.write(1, 0);
    assert(cpu.intRegister.read(1) == 0);
    executeUnit.stage(add);
    executeUnit.tick();
    assert(masterEventQueue.top()->type == "WorkCompleted");
    assert(masterEventQueue.top()->time == add->executionTime * SIM_CYCLES_PER_CPU);
    assert(masterEventQueue.top()->device == &executeUnit);

    executeUnit.process(masterEventQueue.pop());
    testPipeline.tick();

    assert(cpu.intRegister.read(1) == 8);
}