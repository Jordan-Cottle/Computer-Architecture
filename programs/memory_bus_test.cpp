#include "test.h"
using namespace Simulation;

#include "memory_bus.h"

int main()
{
    MemoryController *testMemory = new MemoryController(11, 32, {8, 16, 32});
    testMemory->write(0, MFMT(PI));
    MemoryBus bus = MemoryBus(BUS_ARBITRATION_TIME, testMemory);

    // Memory with 3 partitions should generate three memory bus request queues
    assert(bus.requests.size() == 3);

    // Bus should accept all memory requests
    uint32_t address = 0;
    MemoryRequest writeRequest = MemoryRequest(address, &testPipeline, false);
    MemoryRequest readRequest = MemoryRequest(address, &fetchUnit);

    bool accepted = bus.request(&writeRequest);
    assert(accepted);
    accepted = bus.request(&readRequest);
    assert(accepted);

    // Port for this request should be same as partition in memory
    uint32_t port = bus.port(address);
    assert(port == testMemory->partition(address));

    // Memory requests all get queued
    auto requestQueue = bus.requests.at(port);
    assert(requestQueue->size() == 2);

    assert(requestQueue->front()->address == 0);
    assert(requestQueue->front()->device == &testPipeline);
    assert(requestQueue->front()->read == false); // First request is a write

    simulationClock.cycle += BUS_ARBITRATION_TIME;
    while (simulationClock.cycle < BUS_ARBITRATION_TIME + testMemory->accessTime)
    {
        std::cout << masterEventQueue << "\n";
        masterEventQueue.tick(simulationClock.cycle);
        std::cout << "Requests left: " << requestQueue->size() << "\n";
        assert(requestQueue->front()->inProgress); // Request should be in progress.
        assert(testPipeline.lastEvent == NULL);
        simulationClock.tick();
    }

    std::cout << simulationClock << "\n";

    // Memory should be ready now
    simulationClock.cycle = BUS_ARBITRATION_TIME + testMemory->accessTime;
    std::cout << masterEventQueue << "\n";
    Event *nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == simulationClock.cycle);
    nextEvent->device->process(nextEvent);

    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryWriteReady");
    assert(nextEvent->time == simulationClock.cycle);
    assert(nextEvent->device == &testPipeline);
    nextEvent->device->process(nextEvent);

    // Pipeline received the memory read
    assert(testPipeline.lastEvent->type == "MemoryWriteReady");

    // Write a value to memory through the bus
    bus.write(0, MFMT(PI));
    assert(testMemory->readFloat(0) == PI);
    assert(requestQueue->size() == 1);          // Memory request should be cleared
    assert(!requestQueue->front()->inProgress); // Next request should be ready, but not yet started.

    // Finish processing events in this cycle
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "ProcessRequests");
    assert(nextEvent->device == &bus);
    simulationClock.cycle = nextEvent->time;

    nextEvent->device->process(nextEvent);
    assert(requestQueue->front()->inProgress); // Next request should be started.

    // Remove next bus event, it is no longer needed
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "ProcessRequests");
    assert(nextEvent->time == simulationClock.cycle + bus.accessTime);
    assert(nextEvent->device == &bus);

    // Second memory access scheduled
    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == simulationClock.cycle + testMemory->accessTime);
    simulationClock.cycle += testMemory->accessTime;
    nextEvent->device->process(nextEvent);

    nextEvent = masterEventQueue.pop();
    assert(nextEvent->type == "MemoryReadReady");
    assert(nextEvent->time == simulationClock.cycle);
    assert(nextEvent->device == &fetchUnit); // Fetch unit originally made read request
    testPipeline.process(nextEvent);         // Send event to test pipeline instead
    assert(testPipeline.lastEvent->type == "MemoryReadReady");
    assert(testPipeline.lastEvent->time == simulationClock.cycle);

    uint32_t partition = bus.memory->partition(0);
    assert(bus.memory->memoryBanks.at(partition)->busy());
    assert(bus.readFloat(0) == PI);
    assert(!bus.memory->memoryBanks.at(partition)->busy());
}