#include "test.h"
using namespace Simulation;

#include "memory_instruction.h"

int main()
{
    cpu.addPipeline(&decodeUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.memory.read<uint32_t>(0));

    decodeUnit.stage(instruction);
    decodeUnit.tick();

    // Check that instruction made it to next stage
    assert(testPipeline.staged() != NULL);

    // Check instruction was properly decoded into a Load
    Load *load = dynamic_cast<Load *>(testPipeline.staged());
    assert(load != NULL);
    assert(load->isFp);
    assert(load->keyword() == "flw");
    testPipeline.tick();

    return 0;
}