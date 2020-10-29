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

Instruction *Decode::decode(Instruction *instruction)
{
    std::string op = instruction->operation;

    if (op == "fsd")
    {
        return new Store(instruction, &cpu->intRegister);
    }
    else if (op == "fld")
    {
        return new Load(instruction, &cpu->intRegister);
    }
    else if (op == "fadd.d")
    {
        return new Add(instruction);
    }
    else if (op == "addi")
    {
        return new Add(instruction, instruction->arguments[2]);
    }
    else if (op == "bne")
    {
        Branch *branch = dynamic_cast<Branch *>(instruction);
        int destination = this->cpu->program->index(branch->label);
        return new Bne(branch, destination);
    }
    else if (op == "halt")
    {
        return instruction;
    }

    throw std::runtime_error("Unrecognized instruction " + str(instruction));
}

void Decode::tick()
{
    Instruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction to decode\n";
    }
    else
    {
        std::cout << "Decode processing instruction: " << instruction << "\n";
        Instruction *decodedInstruction = this->decode(instruction);

        this->next->stage(decodedInstruction);
    }
}
