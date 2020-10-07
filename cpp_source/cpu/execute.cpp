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
    if (this->staged() == NULL)
    {
        return;
    }

    Store *store = dynamic_cast<Store *>(this->staged());
    if (store != NULL)
    {
        PipelineInsertEvent *new_event = new PipelineInsertEvent(time + 1, this->next, store);
        eventQueue->push(new_event);
    }
    else
    {
        DecodedInstruction *instruction = dynamic_cast<DecodedInstruction *>(this->staged());

        instruction->execute(this->cpu);
    }

    Pipeline::tick(time, eventQueue);
}
