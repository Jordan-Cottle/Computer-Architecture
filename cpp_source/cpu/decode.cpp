/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "decode.h"

#include "cpu.h"
#include "memory_instruction.h"
#include "arithmetic_instruction.h"

Decode::Decode(Cpu *cpu) : Pipeline("Decode")
{
    this->cpu = cpu;
}

Instruction *Decode::decode(Instruction *instruction)
{
    std::string op = instruction->operation;

    if (op == "fsd")
    {
        return new Store<double>(instruction, &cpu->intRegister);
    }
    else if (op == "fld")
    {
        return new Load<double>(instruction, &cpu->intRegister);
    }
    else if (op == "fadd.d")
    {
        return new Add<double>(instruction);
    }
    else if (op == "addi")
    {
        return new Add<int>(instruction, instruction->arguments[2]);
    }

    throw std::runtime_error("Unrecognized instruction " + str(instruction));
}

void Decode::tick(ulong time, EventQueue *eventQueue)
{
    Instruction *staged = this->staged();

    if (staged == NULL)
    {
        std::cout << "No instruction to decode\n";
    }
    else
    {
        std::cout << "Decoding: " << this->staged() << "\n";
        Instruction *decodedInstruction = this->decode(staged);

        PipelineInsertEvent *new_event = new PipelineInsertEvent(time + 1, this->next, decodedInstruction);

        eventQueue->push(new_event);
    }

    Pipeline::tick(time, eventQueue);
}
