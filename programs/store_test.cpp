#include "test.h"
using namespace Simulation;

#include "memory_instruction.h"

int main()
{

    const int RAM_LOCATION = 100;
    cpu.intRegister.write(1, RAM_LOCATION);

    cpu.fpRegister.write(3, PI); // Pi

    cpu.addPipeline(&storeUnit);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.memory->read<uint32_t>(12));
    Store *storeInstruction = new Store(instruction);

    storeUnit.stage(storeInstruction);
    storeUnit.tick();
    std::cout << masterEventQueue << "\n";
    assert(masterEventQueue.top()->type == "MemoryRequest");

    masterEventQueue.tick(0);
    std::cout << masterEventQueue << "\n";
    assert(masterEventQueue.top()->type == "MemoryReady");
    assert(masterEventQueue.top()->time == (ulong)cpu.memory->accessTime);
    assert(masterEventQueue.top()->device == &storeUnit);

    simulationClock.cycle = cpu.memory->accessTime;
    storeUnit.process(masterEventQueue.pop());
    assert(masterEventQueue.top()->type == "WorkCompleted");
    assert(masterEventQueue.top()->time == (ulong)cpu.memory->accessTime);
    assert(masterEventQueue.top()->device == &storeUnit);

    assert(cpu.memory->read<float>(RAM_LOCATION - 4) == 0);
    storeUnit.process(masterEventQueue.pop());
    assert(cpu.memory->read<float>(RAM_LOCATION - 4) == PI);

    return 0;
}