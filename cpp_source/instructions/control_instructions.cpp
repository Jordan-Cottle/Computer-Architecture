/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "control_instructions.h"
#include "cpu.h"

#include "opcodes.h"

#include "simulation.h"

ControlInstruction::ControlInstruction(RawInstruction *instruction) : DecodedInstruction(instruction)
{
}

bool ControlInstruction::take(Cpu *cpu)
{
    return true; // Default branch behavior is to always take
}

int ControlInstruction::offset(Cpu *cpu)
{
    return sign_extend(getImmediateSB(this->data), 12);
}

void ControlInstruction::execute(Cpu *cpu)
{
    bool take = this->take(cpu);
    if (this->take(cpu) != cpu->branchSpeculated)
    {
        std::cout << "Incorrect branch prediction!\n";
        cpu->flush();

        if (take)
        {
            std::cout << "Jumping by " << str(this->offset(cpu)) << "\n";
            cpu->programCounter.jump(this->offset(cpu));
        }
        else
        {
            cpu->programCounter.value = cpu->jumpedFrom + MEMORY_ADDRESSES_PER_INSTRUCTION;
        }
    }

    // Fetch unit has already made a correct prediction, nothing to do
    // TODO actual PC should be tracked separate from fetch unit's predictions
}

std::string ControlInstruction::__str__()
{
    return DecodedInstruction::__str__();
}

BranchInstruction::BranchInstruction(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->leftIndex = getR1(instruction->data);
    this->rightIndex = getR2(instruction->data);
}

Bne::Bne(RawInstruction *instruction) : BranchInstruction(instruction)
{
}

bool Bne::take(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    int right = cpu->intRegister.read(this->rightIndex);
    std::cout << "Jumping by " << str(this->offset(cpu)) << " if " << str(left) << " != " << str(right) << "\n";
    return left != right;
}

std::string Bne::__str__()
{
    return ControlInstruction::__str__() + " if R" + str(this->leftIndex) + " != R" + str(this->rightIndex);
}

Blt::Blt(RawInstruction *instruction) : BranchInstruction(instruction)
{
}

bool Blt::take(Cpu *cpu)
{
    int left = cpu->intRegister.read(this->leftIndex);
    int right = cpu->intRegister.read(this->rightIndex);
    std::cout << "Jumping by " << str(this->offset(cpu)) << " if " << str(left) << " < " << str(right) << "\n";
    return left < right;
}

std::string Blt::__str__()
{
    return ControlInstruction::__str__() + " if R" + str(this->leftIndex) + " < R" + str(this->rightIndex);
}

Jump::Jump(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->registerIndex = getRd(instruction->data);
}

int Jump::offset(Cpu *cpu)
{
    return sign_extend(getImmediateUB(this->data), 20);
}

void Jump::execute(Cpu *cpu)
{
    ControlInstruction::execute(cpu);
    cpu->intRegister.write(this->registerIndex, cpu->jumpedFrom + MEMORY_ADDRESSES_PER_INSTRUCTION);
}

Jalr::Jalr(RawInstruction *instruction) : ControlInstruction(instruction)
{
    this->registerIndex = getRd(instruction->data);
    this->sourceIndex = getR1(instruction->data);
}

int Jalr::offset(Cpu *cpu)
{
    return sign_extend(getImmediateI(this->data), 12) + cpu->intRegister.read(this->sourceIndex);
}

void Jalr::execute(Cpu *cpu)
{
    if (this->offset(cpu) == 0)
    {
        // Is this how we're supposed to end the program??
        std::cout << "Return to PC 0 detected, program complete\n";
        cpu->complete = true;
        return;
    }

    ControlInstruction::execute(cpu);
    cpu->intRegister.write(this->registerIndex, cpu->jumpedFrom + MEMORY_ADDRESSES_PER_INSTRUCTION);
}
