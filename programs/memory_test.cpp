#include "test.h"
using namespace Simulation;

int main()
{
    Memory memory = Memory(20, 8, {0x4, 0x8});

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
    assert(memory.busy[0] == true);
    accepted = memory.request(&request);
    assert(!accepted);

    assert(masterEventQueue.events.top()->type == "MemoryReadReady");
    assert(masterEventQueue.events.top()->time == (ulong)memory.accessTime);
    assert(masterEventQueue.events.top()->device == &testPipeline);

    return 0;
}