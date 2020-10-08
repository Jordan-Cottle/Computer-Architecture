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

    Store *store = dynamic_cast<Store *>(instruction);
    if (store != NULL)
    {
        std::cout << "Passing store on from execute stage\n";
        PipelineInsertEvent *new_event = new PipelineInsertEvent(time + 1, this->next, store);
        eventQueue->push(new_event);
    }
    else if (instruction->operation == "stall")
    {
        std::cout << "Executing stall\n";
        return;
    }
    else
    {
        std::cout << "Executing " << instruction << "\n";
        DecodedInstruction *decoded = dynamic_cast<DecodedInstruction *>(instruction);

        decoded->execute(this->cpu);
    }
}
