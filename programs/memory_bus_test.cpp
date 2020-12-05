#include "test.h"
using namespace Simulation;

#include "memory_bus.h"

int main()
{
    Memory *testMemory = new Memory(10, 32, {8, 16, 32});
    testMemory->write(0, MFMT(PI));
    MemoryBus bus = MemoryBus(BUS_ARBITRATION_TIME, testMemory);

    // Memory with 3 partitions should generate three memory bus request queues
    assert(bus.requests.size() == 3);

    // Bus should accept all memory requests
    uint32_t address = 0;
    bool accepted = bus.request(address, &testPipeline, false);
    assert(accepted);
    accepted = bus.request(address, &fetchUnit);
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
        assert(requestQueue->front()->requested); // Request should be in progress.
        assert(testPipeline.lastEvent == NULL);
        simulationClock.tick();
    }

    std::cout << simulationClock << "\n";

    // Memory should be ready now
    simulationClock.cycle = BUS_ARBITRATION_TIME + testMemory->accessTime;
    assert(masterEventQueue.top()->type == "MemoryWriteReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    testPipeline.process(masterEventQueue.pop());

    // Pipeline received the memory read
    assert(testPipeline.lastEvent->type == "MemoryWriteReady");

    // Write a value to memory through the bus
    bus.write(0, (void *)&PI, sizeof(PI));
    assert(testMemory->readFloat(0) == PI);
    assert(requestQueue->size() == 1);         // Memory request should be cleared
    assert(!requestQueue->front()->requested); // Next request should be ready, but not yet started.

    // Finish processing events in this cycle
    assert(masterEventQueue.top()->type == "ProcessRequests");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    masterEventQueue.tick(simulationClock.cycle);
    assert(requestQueue->front()->requested); // Next request should be started.

    // Remove process requests since test no longer needs it. This effectively kills the memory bus until it is started again
    assert(masterEventQueue.top()->type == "ProcessRequests"); // Bus should keep trying to process requests
    assert(masterEventQueue.top()->time == simulationClock.cycle + bus.accessTime);
    masterEventQueue.pop();

    // Second memory access scheduled
    assert(masterEventQueue.top()->type == "MemoryReadReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle + testMemory->accessTime);

    simulationClock.cycle += testMemory->accessTime;
    assert(masterEventQueue.top()->type == "MemoryReadReady");
    assert(masterEventQueue.top()->time == simulationClock.cycle);
    assert(masterEventQueue.top()->device == &fetchUnit); // Fetch unit originally made read request
    testPipeline.process(masterEventQueue.pop());         // Send event to test pipeline instead
    assert(testPipeline.lastEvent->type == "MemoryReadReady");
    assert(testPipeline.lastEvent->time == simulationClock.cycle);

    assert(bus.memory->busy[bus.memory->partition(0)] == true);
    assert(bus.readFloat(0) == PI);
    assert(bus.memory->busy[bus.memory->partition(0)] == false);
}