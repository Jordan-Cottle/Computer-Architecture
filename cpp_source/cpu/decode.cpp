/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "decode.h"

#include "memory_instruction.h"
#include "arithmetic_instruction.h"
#include "control_instructions.h"

#include "simulation.h"
using namespace Simulation;

Decode::Decode(Cpu *cpu) : Pipeline("Decode")
{
    this->cpu = cpu;
}

DecodedInstruction *Decode::decode(RawInstruction *instruction)
{
    std::string op = instruction->keyword();

    if (op == "fsw")
    {
        return new Store(instruction);
    }
    else if (op == "sw")
    {
        return new Store(instruction);
    }
    else if (op == "flw")
    {
        return new Load(instruction);
    }
    else if (op == "lw")
    {
        return new Load(instruction);
    }
    else if (op == "fadd.s")
    {
        return new Add(instruction);
    }
    else if (op == "addi")
    {
        return new Add(instruction);
    }
    else if (op == "lui")
    {
        return new Lui(instruction);
    }
    else if (op == "add")
    {
        return new Add(instruction);
    }
    else if (op == "slli")
    {
        return new Slli(instruction);
    }
    else if (op == "bne")
    {
        return new Bne(instruction);
    }
    else if (op == "blt")
    {
        return new Blt(instruction);
    }
    else if (op == "jal")
    {
        return new Jump(instruction);
    }
    else if (op == "jalr")
    {
        return new Jalr(instruction);
    }

    throw std::runtime_error("Unrecognized instruction " + op + " " + str(instruction));
}

void Decode::tick()
{
    Pipeline::tick();

    if (this->free())
    {
        std::cout << "No instruction to decode\n";
        return;
    }
    if (this->next->busy())
    {
        std::cout << "Decode waiting because next stage is busy\n";
        return;
    }
    if (this->busy())
    {
        std::cout << "Decode continuing to work on its task\n";
        return;
    }
    this->_busy = true;

    RawInstruction *instruction = this->staged();

    std::cout << "Decode processing instruction: " << instruction << "\n";

    DecodedInstruction *decodedInstruction = this->decode(instruction);
    delete instruction; // All data has been saved to decodedInstruction

    this->next->stage(decodedInstruction);
    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
