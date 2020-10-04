#include <iostream>
#include <stdlib.h>

#include "event_queue.h"
#include "event.h"

#include "instruction.h"
#include "instruction_queue.h"

#include "device.h"
#include "clock.h"

void event_processing_test()
{
    SimulationDevice device = SimulationDevice("TestDevice");
    EventQueue masterEventQueue;
    Clock clock;

    for (int i = 1; i <= 20; i++)
    {
        Event *event = new Event(rand() % i, rand() % 10 + 1);

        masterEventQueue.push(event);
    }

    std::cout << "Starting simulation...\n";

    while (clock.cycle < 100) // Run for 100 cycles
    {
        while (masterEventQueue.nextTime() == clock.cycle)
        {
            Event *event = masterEventQueue.pop();
            Event *new_event = device.process(event);

            masterEventQueue.push(new_event);
        }

        clock.tick();
    }

    std::cout << "Master Event Queue: " << masterEventQueue << "\n";

    std::cout << device << " processed " << device.processed_count << " events\n";
}

void instructionQueueTest()
{
    std::vector<Instruction> instructions = std::vector<Instruction>();
    for (int i = 0; i < 10; i++)
    {
        instructions.push_back(Instruction("ADD", {"R" + std::to_string(i), "R" + std::to_string(i), "R" + std::to_string(i + 1)}));
    }

    InstructionQueue instructionQueue = InstructionQueue(instructions);

    std::cout << instructionQueue << "\n";

    for (int i = 0; i < 3; i++)
    {
        Instruction next = instructionQueue.next();

        std::cout << "Processing: " << next << "\n";
    }

    std::cout << instructionQueue << "\n";
}

int main()
{
    event_processing_test();
    return 0;
}