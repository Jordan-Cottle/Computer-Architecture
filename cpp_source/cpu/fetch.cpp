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
    this->outstandingRequest = false;
}

void Fetch::tick()
{
    Pipeline::tick();

    if (this->next->busy())
    {
        // std::cout << "Fetch waiting because next stage is busy\n";
        return;
    }
    if (this->busy())
    {
        // std::cout << "Fetch continuing to work on its task\n";
        return;
    }
    if (this->outstandingRequest)
    {
        std::cout << "Fetch unit not requesting because it already has an outstanding request\n";
        return;
    }

    std::cout << "Requesting new instruction from memory\n";
    Event *event = new Event("MemoryRequest", simulationClock.cycle, this);
    masterEventQueue.push(event);
    this->outstandingRequest = true;

    this->_busy = true;
}

void Fetch::processInstruction()
{
    RawInstruction *instruction = this->staged();

    std::cout << "Fetch processing instruction: " << instruction << "\n";

    uint32_t opcode = getOpcode(instruction->data);
    // TODO move branch predicting logic into a branch prediction unit
    if (opcode == 0b1100011)
    { // Branch detected, attempt a prediction

        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;
        ControlInstruction branch = ControlInstruction(instruction);
        this->cpu->programCounter.jump(branch.offset(this->cpu));

        std::cout << "Branch by " << this->cpu->programCounter << " predicted\n";
    }
    else if (opcode == 0b1101111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jump jump = Jump(instruction);
        std::cout << "Jump by " << jump.offset(this->cpu) << " detected\n";

        this->cpu->programCounter.jump(jump.offset(this->cpu));
    }
    else if (opcode == 0b1100111)
    {
        this->cpu->branchSpeculated = true;
        this->cpu->jumpedFrom = this->cpu->programCounter.value;

        Jalr jalr = Jalr(instruction);

        this->cpu->programCounter.jump(jalr.offset(this->cpu));
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
}

void Fetch::process(Event *event)
{
    if (event->type == "MemoryRequest")
    {
        event->handled = true;
        bool accepted = this->cpu->memory->request(this->cpu->programCounter.value, this);
        if (!accepted)
        {
            Event *event = new Event("MemoryRequest", simulationClock.cycle + 5, this);
            masterEventQueue.push(event);
        }
    }
    else if (event->type == "MemoryReady")
    {
        event->handled = true;

        this->stage(new RawInstruction(this->cpu->memory->readUint(this->cpu->programCounter.value)));

        Event *event = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
        masterEventQueue.push(event);
        this->outstandingRequest = false;
    }
    else if (event->type == "WorkCompleted")
    {
        event->handled = true;
        this->processInstruction();
    }

    Pipeline::process(event);
}