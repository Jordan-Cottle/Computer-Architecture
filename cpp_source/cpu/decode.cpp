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

    throw std::runtime_error("Unrecognized instruction " + op + " " + str(instruction));
}

void Decode::tick()
{
    RawInstruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to decode\n";
    }
    else
    {
        std::cout << "Decode processing instruction: " << instruction << "\n";
        DecodedInstruction *decodedInstruction = this->decode(instruction);
        delete instruction; // All data has been saved to decodedInstruction

        this->next->stage(decodedInstruction);
    }
}
