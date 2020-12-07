#include "test.h"
using namespace Simulation;

int main()
{
    MemoryController memory = MemoryController(20, 8, {0x4, 0x8});

    for (int i = 0; i < 32; i++)
    {
        int num = 1 << i;
        memory.write(0, MFMT(num));
        assert(memory.readInt(0) == num);
    }

    float num = .1f;
    memory.write(4, MFMT(num));
    assert(memory.readFloat(4) == num);

    uint32_t partition = memory.partition(0);
    assert(partition == 0);
    partition = memory.partition(3);
    assert(partition == 0);
    partition = memory.partition(4);
    assert(partition == 1);
    partition = memory.partition(7);
    assert(partition == 1);

    MemoryRequest request = MemoryRequest(0, &testPipeline);
    bool accepted = memory.request(&request);
    assert(accepted);
    assert(memory.memoryBanks.at(0)->busy());
    MemoryRequest otherRequest = MemoryRequest(0, &testPipeline);
    accepted = memory.request(&otherRequest);
    assert(!accepted);

    Event *nextEvent = masterEventQueue.top();
    simulationClock.cycle = nextEvent->time;
    assert(nextEvent->type == "MemoryReady");
    assert(nextEvent->time == (ulong)memory.accessTime);
    assert(nextEvent->device == memory.memoryBanks.at(0));
    nextEvent->device->process(masterEventQueue.pop());
    nextEvent = masterEventQueue.top();
    assert(nextEvent->type == "MemoryReadReady");
    assert(nextEvent->time == (ulong)memory.accessTime);
    assert(nextEvent->device == &testPipeline);
    nextEvent->device->process(masterEventQueue.pop());
    nextEvent = masterEventQueue.top();
    memory.readUint(0);
    assert(!memory.memoryBanks.at(0)->busy());

    // Requests to separate banks get handled in parallel
    request = MemoryRequest(0, &testPipeline);
    accepted = memory.request(&request);
    assert(accepted);
    assert(memory.memoryBanks.at(0)->busy());
    otherRequest = MemoryRequest(4, &testPipeline);
    accepted = memory.request(&otherRequest);
    assert(accepted);
    assert(memory.memoryBanks.at(1)->busy());

    assert(masterEventQueue.size() == 2);
    uint32_t i = 0;
    while (!masterEventQueue.empty())
    {
        nextEvent = masterEventQueue.pop();
        assert(nextEvent->type == "MemoryReady");
        assert(nextEvent->time == simulationClock.cycle + memory.accessTime);
        assert(nextEvent->device == memory.memoryBanks.at(i++));
    }
    return 0;
}