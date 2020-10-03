#include "event_queue.h"
#include "event.h"

#include "instruction.h"
#include "instruction_queue.h"

#include <iostream>
#include <stdlib.h>

void assignment_1()
{
    EventQueue masterEventQueue;

    for (int i = 1; i <= 20; i++)
    {
        Event *event = new Event(rand() % i, rand() % 10 + 1);

        masterEventQueue.push(event);
    }

    std::cout << "Starting simulation...\n";

    for (int i = 0; i < 100; i++)
    {
        Event *event = masterEventQueue.pop();
        Event *new_event = event->process();

        masterEventQueue.push(new_event);
    }

    std::cout << "Master Event Queue: " << str(masterEventQueue);
}

int main()
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
    return 0;
}