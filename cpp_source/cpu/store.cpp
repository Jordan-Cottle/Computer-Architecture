/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "store.h"

#include "memory_instruction.h"

#include "cpu.h"

StorePipeline::StorePipeline(Cpu *cpu) : Pipeline("StorePipeline")
{
    this->cpu = cpu;
}

void StorePipeline::tick()
{
    Instruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to store\n";
        return;
    }

    std::cout << "Store processing instruction: " << instruction << "\n";
    if (instruction->operation == "halt")
    {
        this->cpu->complete = true;
        return;
    }

    Store *store = dynamic_cast<Store *>(instruction);

    if (store == NULL)
    {
        throw std::runtime_error("Store pipeline stage only accepts store instructions");
    }

    store->execute(this->cpu);
}
