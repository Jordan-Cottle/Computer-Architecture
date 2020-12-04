#include "test.h"
using namespace Simulation;

#include "memory_bus.h"

int main()
{
    cpu.memory->write(0, (void *)&PI, sizeof(PI));
    MemoryBus bus = MemoryBus(BUS_ARBITRATION_TIME, (Memory *)cpu.memory);

    // Bus should accept all memory requests
    bool accepted = bus.request(0, &testPipeline, false);
    assert(accepted);
    accepted = bus.request(0, &fetchUnit);
    assert(accepted);

    // Memory requests all get queued
    assert(bus.requests.size() == 2);
    assert(masterEventQueue.events.size() == 2);
    assert(bus.requests.top()->address == 0);
    assert(bus.requests.top()->device == &testPipeline);
    assert(bus.requests.top()->completeAt == BUS_ARBITRATION_TIME);

    simulationClock.cycle += BUS_ARBITRATION_TIME;
    while (simulationClock.cycle < BUS_ARBITRATION_TIME + cpu.memory->accessTime)
    {
        assert(masterEventQueue.top()->time == simulationClock.cycle);
        masterEventQueue.tick(simulationClock.cycle);
        assert(bus.requests.size() == 1);
        assert(masterEventQueue.events.size() == 2);
        assert(testPipeline.lastEvent == NULL);

        // Rescheduled event
        assert(masterEventQueue.top()->time == simulationClock.cycle + BUS_ARBITRATION_TIME);
        simulationClock.cycle += BUS_ARBITRATION_TIME;
    }

    // Memory should be ready now
    simulationClock.cycle = BUS_ARBITRATION_TIME + cpu.memory->accessTime;
    assert(masterEventQueue.top()->type == "MemoryWriteReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    testPipeline.process(masterEventQueue.pop());
    // Pipeline received the memory read
    assert(testPipeline.lastEvent->type == "MemoryWriteReady");
    // Write a value to memory through the bus
    bus.write(0, (void *)&PI, sizeof(PI));
    assert(cpu.memory->readFloat(0) == PI);

    // Finish processing events in this cycle
    assert(masterEventQueue.top()->type == "ProcessRequest");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    masterEventQueue.tick(simulationClock.cycle);

    assert(bus.requests.size() == 0);
    // Should be just a MemoryReady event for the second memory request
    assert(masterEventQueue.events.size() == 1);
    // Second memory access scheduled
    assert(masterEventQueue.top()->type == "MemoryReadReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle + cpu.memory->accessTime);

    simulationClock.cycle += cpu.memory->accessTime;
    assert(masterEventQueue.top()->type == "MemoryReadReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    testPipeline.process(masterEventQueue.pop());
    assert(testPipeline.lastEvent->type == "MemoryReadReady");
    assert(testPipeline.lastEvent->time == simulationClock.cycle);

    assert(bus.memory->busy[bus.memory->partition(0)] == true);
    assert(bus.readFloat(0) == PI);
    assert(bus.memory->busy[bus.memory->partition(0)] == false);
}