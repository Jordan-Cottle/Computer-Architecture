/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "execute.h"

#include "memory_instruction.h"

Execute::Execute(Cpu *cpu) : Pipeline("Execute")
{
    this->cpu = cpu;
}

void Execute::tick(ulong time, EventQueue *eventQueue)
{
    Instruction *instruction = this->staged();
    Pipeline::tick(time, eventQueue);

    if (instruction == NULL)
    {
        std::cout << "No instruction to execute\n";
        return;
    }

    std::cout << "Execute processing instruction: " << instruction << "\n";
    Store *store = dynamic_cast<Store *>(instruction);
    if (store != NULL || instruction->operation == "halt")
    {
        PipelineInsertEvent *new_event = new PipelineInsertEvent(time + 1, this->next, instruction);
        eventQueue->push(new_event);
    }
    else
    {
        DecodedInstruction *decoded = dynamic_cast<DecodedInstruction *>(instruction);

        decoded->execute(this->cpu);
    }
}
