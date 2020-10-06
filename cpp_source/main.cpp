#include <iostream>
#include <stdlib.h>

#include "event_queue.h"
#include "event.h"

#include "instruction.h"
#include "instruction_queue.h"

#include "device.h"
#include "clock.h"

#include "sim_register.h"
#include "pipeline.h"

void instructionQueueTest()
{
    std::vector<Instruction *> instructions = std::vector<Instruction *>();
    for (int i = 0; i < 10; i++)
    {
        instructions.push_back(new Instruction("ADD", {"R" + str(i), "R" + str(i), "R" + str(i + 1)}));
    }

    InstructionQueue instructionQueue = InstructionQueue(instructions);

    std::cout << instructionQueue << "\n";

    for (int i = 0; i < 3; i++)
    {
        Instruction *next = instructionQueue.next();

        std::cout << "Processing: " << next << "\n";
        delete next;
    }

    std::cout << instructionQueue << "\n";
}

void memory_test()
{
    Register<Event *> memory = Register<Event *>(2, "EventStorage");

    Pipeline *p = new Pipeline("TestPipeline");

    Event *e = new Event(0, p);
    Event *f = new Event(0, p);

    memory.write(0, e);
    memory.write(1, f);

    std::cout << memory << "\n\n";
    memory.clear(1);
    std::cout << memory << "\n\n";
    memory.clear();
    std::cout << memory << "\n\n";

    delete e;
    delete f;
}

void pipeline_test()
{
    Clock clock;
    std::vector<Pipeline *> pipelines = std::vector<Pipeline *>(2);

    pipelines[1] = new Pipeline("SecondStage");
    pipelines[0] = new Pipeline("FirstStage", pipelines[1]);

    Instruction instruction = Instruction("ADD", {"0"});
    EventQueue queue;
    Event *event = new PipelineInsertEvent(0, &instruction, pipelines[0]);
    queue.push(event);

    while (clock.cycle < 6)
    {
        std::cout << clock << "\n";

        while (queue.top() != NULL && queue.top()->time == clock.cycle)
        {
            event = queue.pop();
            SimulationDevice *device = dynamic_cast<SimulationDevice *>(event->device);
            device->process(event, &queue);
        }

        for (auto pipeline : pipelines)
        {
            pipeline->tick();
            std::cout << pipeline << "\n";
        }

        clock.tick();
    }
}
int main()
{
    instructionQueueTest();
    return 0;
}