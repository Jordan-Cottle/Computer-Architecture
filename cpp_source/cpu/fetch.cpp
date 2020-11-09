/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "fetch.h"

#include "opcodes.h"
#include "control_instructions.h"

#include "simulation.h"
using namespace Simulation;

Fetch::Fetch(Cpu *cpu) : Pipeline("Fetch")
{
    this->cpu = cpu;
}

void Fetch::tick()
{
    Pipeline::tick();

    if (this->next->busy())
    {
        std::cout << "Fetch waiting because next stage is busy\n";
        return;
    }
    if (this->busy())
    {
        std::cout << "Fetch continuing to work on its task\n";
        return;
    }
    if (this->free())
    {
        std::cout << "Fetching new instruction\n";
        this->stage(new RawInstruction(this->cpu->memory.read<uint32_t>(this->cpu->programCounter.value)));
    }
    this->_busy = true;

    RawInstruction *instruction = this->staged();

    std::cout << "Fetch processing instruction: " << instruction << "\n";

    uint32_t opcode = getOpcode(instruction->data);
    // TODO move branch predicting logic into a branch prediction unit
    if (opcode == 0b1100011)
    { // Branch detected, attempt a prediction

        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;
        this->cpu->programCounter.jump(getImmediateSB(instruction->data));

        std::cout << "Branch by " << this->cpu->programCounter << " predicted\n";
    }
    else if (opcode == 0b1101111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jump jump = Jump(instruction);
        std::cout << "Jump by " << jump.offset << " detected\n";

        this->cpu->programCounter.jump(jump.offset);
    }
    else if (opcode == 0b1100111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jalr jalr = Jalr(instruction);

        int offset = jalr.offset + cpu->intRegister.read(jalr.sourceIndex);
        this->cpu->programCounter.jump(offset);
        std::cout << "Jalr to " << this->cpu->programCounter << " detected\n";
    }
    else
    {
        ++this->cpu->programCounter;
    }

    // Stop fetching if halt is encountered
    if (instruction->data != 0) // 0 is an invalid code in risc-v
    {
        this->next->stage(instruction);
    }
    else
    {
        // Give downstream pipelines time to complete
        Event *complete = new Event("Complete", simulationClock.cycle + this->cpu->pipelines.size(), this->cpu);
        masterEventQueue.push(complete);
    }

    Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
    masterEventQueue.push(workCompleted);
}
