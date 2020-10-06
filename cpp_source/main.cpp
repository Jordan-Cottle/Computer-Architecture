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
#include "fetch.h"

EventQueue meq;

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

    Event *e = new Event(0, NULL);
    Event *f = new Event(0, NULL);

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

/*
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
*/

void fetchTest()
{
    Register<Instruction *> instructionMemory = Register<Instruction *>(5);

    Fetch fetchUnit = Fetch(NULL, &instructionMemory);

    instructionMemory.write(0, new Instruction("ADD", {"#0", "1", "2"}));
    instructionMemory.write(1, new Instruction("SUB", {"#1", "2", "1"}));
    instructionMemory.write(2, new Instruction("MULT", {"#2", "3", "3"}));
    instructionMemory.write(3, new Instruction("DIV", {"#3", "6", "3"}));
    instructionMemory.write(4, new Instruction("BRANCH", {"#4", "1"}));

    std::cout << "Instructions " << instructionMemory << "\n";

    for (int i = 0; i < instructionMemory.size; i++)
    {
        FetchEvent *event = new FetchEvent(-1, &fetchUnit, i);
        std::cout << event << "\n";

        meq.push(event);
    }

    std::cout << meq << "\n";

    Clock clock;

    while (!meq.empty())
    {
        std::cout << clock << "\n";
        clock.tick();

        Event *event = meq.pop();

        std::cout << "Fetch unit processing " << event << "\n";
        fetchUnit.process(event, &meq);
        std::cout << fetchUnit << "\n";

        fetchUnit.tick(clock.cycle, &meq);
    }
}
int main()
{
    fetchTest();
    return 0;
}