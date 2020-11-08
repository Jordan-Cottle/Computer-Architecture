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
    RawInstruction *instruction = this->staged();
    Pipeline::tick();

    if (instruction == NULL)
    {
        std::cout << "No instruction fetched\n";
        return;
    }

    std::cout << "Fetch processing instruction: " << instruction << "\n";

    uint32_t opcode = getOpcode(instruction->data);
    // TODO move branch predicting logic into a branch prediction unit
    if (opcode == 0b1100011)
    { // Branch detected, attempt a prediction

        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;
        this->cpu->programCounter.jump(getImmediateSB(instruction->data));

        std::cout << "Branch to " << this->cpu->programCounter << " predicted\n";
    }
    else if (opcode == 0b1101111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jump jump = Jump(instruction);
        std::cout << "Jump to " << jump.destination << " detected\n";

        this->cpu->programCounter.jump(jump.destination);
    }
    else if (opcode == 0b1100111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jalr jalr = Jalr(instruction);

        int destination = jalr.destination + cpu->intRegister.read(jalr.sourceIndex);
        this->cpu->programCounter.jump(destination);
        std::cout << "Jalr to " << this->cpu->programCounter << " detected\n";
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
}

void Fetch::process(Event *event)
{
    if (event->type == "Fetch")
    {
        event->handled = true;
        this->stage(new RawInstruction(this->cpu->ram.read<uint32_t>(this->cpu->programCounter.value)));
        ++this->cpu->programCounter;
    }

    Pipeline::process(event);
}
