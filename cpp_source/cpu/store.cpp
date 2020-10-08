/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "store.h"

#include "memory_instruction.h"

StorePipeline::StorePipeline(Cpu *cpu) : Pipeline("StorePipeline")
{
    this->cpu = cpu;
}

void StorePipeline::tick(ulong time, EventQueue *eventQueue)
{
    if (this->staged() == NULL)
    {
        return;
    }

    Store *store = dynamic_cast<Store *>(this->staged());

    if (store == NULL)
    {
        throw std::runtime_error("Store pipeline stage only accepts store instructions");
    }

    store->execute(this->cpu);

    Pipeline::tick(time, eventQueue);
}
