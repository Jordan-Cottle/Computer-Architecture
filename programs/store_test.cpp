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

    RawInstruction *instruction = new RawInstruction(cpu.memory->readUint(12));
    Store *storeInstruction = new Store(instruction);

    storeUnit.stage(storeInstruction);
    storeUnit.tick();
    std::cout << masterEventQueue << "\n";
    Event *nextEvent = masterEventQueue.top();
    assert(nextEvent->type == "MemoryRequest");
    assert(nextEvent->time == simulationClock.cycle);
    assert(nextEvent->device == &storeUnit);
    nextEvent->device->process(masterEventQueue.pop());

    nextEvent = masterEventQueue.top();
    std::cout << masterEventQueue << "\n";
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == (ulong)cpu.memory->accessTime);
    simulationClock.cycle = cpu.memory->accessTime;
    nextEvent->device->process(masterEventQueue.pop());
    nextEvent = masterEventQueue.top();

    assert(nextEvent->type == "MemoryWriteReady");
    assert(nextEvent->time == (ulong)cpu.memory->accessTime);
    nextEvent->device->process(masterEventQueue.pop());
    nextEvent = masterEventQueue.top();

    assert(nextEvent->type == "WorkCompleted");
    assert(nextEvent->time == (ulong)cpu.memory->accessTime);
    assert(nextEvent->device == &storeUnit);

    assert(cpu.memory->readFloat(RAM_LOCATION - 4) == 0);
    nextEvent->device->process(masterEventQueue.pop());
    assert(cpu.memory->readFloat(RAM_LOCATION - 4) == PI);

    return 0;
}