#include "test.h"
using namespace Simulation;

int main()
{
    Memory memory = Memory(8, 20, {0x4, 0x4});

    for (int i = 0; i < 32; i++)
    {
        int num = 1 << i;
        memory.write(0, num);
        assert(memory.read<int>(0) == num);
    }

    float num = .1f;
    memory.write(4, num);
    assert(memory.read<float>(4) == num);

    uint32_t partition = memory.partition(0);
    assert(partition == 0);
    partition = memory.partition(3);
    assert(partition == 0);
    partition = memory.partition(4);
    assert(partition == 1);
    partition = memory.partition(7);
    assert(partition == 1);

    bool accepted = memory.request(0, &testPipeline);
    assert(accepted);
    assert(memory.busy[0] == true);
    accepted = memory.request(0, &testPipeline);
    assert(!accepted);

    assert(masterEventQueue.events.top()->type == "MemoryReady");
    assert(masterEventQueue.events.top()->time == (ulong)memory.accessTime);
    assert(masterEventQueue.events.top()->device == &testPipeline);

    return 0;
}