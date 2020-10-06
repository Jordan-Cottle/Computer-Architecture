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
#include "decode.h"

EventQueue meq;

void instructionQueueTest()
{
    std::vector<Instruction *> instructions = std::vector<Instruction *>();
    for (int i = 0; i < 10; i++)
    {
        instructions.push_back(new Instruction("ADD", {i, i, i + 1}));
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

void fetchTest()
{
    Register<Instruction *> instructionMemory = Register<Instruction *>(5);

    Decode decodeUnit = Decode(NULL);
    Fetch fetchUnit = Fetch(&decodeUnit, &instructionMemory);

    instructionMemory.write(0, new Instruction("ADD", {0, 1, 2}));
    instructionMemory.write(1, new Instruction("SUB", {1, 2, 1}));
    instructionMemory.write(2, new Instruction("MULT", {2, 3, 4}));
    instructionMemory.write(3, new Instruction("DIV", {3, 6, 3}));
    instructionMemory.write(4, new Instruction("BRANCH", {4, 1}));

    std::cout << "Instructions " << instructionMemory << "\n";

    for (int i = 0; i < instructionMemory.size; i++)
    {
        FetchEvent *event = new FetchEvent(i, &fetchUnit, i);
        std::cout << event << "\n";

        meq.push(event);
    }

    std::cout << meq << "\n";

    Clock clock;

    while (clock.cycle < 10)
    {
        std::cout << clock << "\n";
        while (!meq.empty() && meq.nextTime() == clock.cycle)
        {
            Event *event = meq.pop();
            SimulationDevice *device = event->device;

            std::cout << "Processing " << event << "\n";
            device->process(event, &meq);
            std::cout << device << "\n";
        }

        std::cout << "Ticking devices:\n";

        fetchUnit.tick(clock.cycle, &meq);
        decodeUnit.tick(clock.cycle, &meq);

        std::cout << fetchUnit << "\n";
        std::cout << decodeUnit << "\n";

        clock.tick();
    }
}
int main()
{
    fetchTest();
    return 0;
}