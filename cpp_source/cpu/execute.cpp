/*
    Author: Jordan Cottle
    Created: 10/07/2020
*/

#include "execute.h"

#include "memory_instruction.h"

#include "simulation.h"
using namespace Simulation;

Execute::Execute(Cpu *cpu) : Pipeline("Execute")
{
    this->cpu = cpu;
}

void Execute::tick()
{
    Pipeline::tick();

    if (this->free())
    {
        DEBUG << "No instruction to execute\n";
        return;
    }
    if (this->next->busy())
    {
        DEBUG << "Execute waiting because next stage is busy\n";
        return;
    }
    if (this->busy())
    {
        DEBUG << "Execute continuing to work on its task\n";
        return;
    }
    this->_busy = true;

    DecodedInstruction *instruction = (DecodedInstruction *)this->staged();
    INFO << "Execute processing instruction: " << instruction << "\n";
    Load *load = dynamic_cast<Load *>(instruction);
    if (load != NULL)
    {
        Event *event = new Event("MemoryRequest", simulationClock.cycle, this, HIGH);
        masterEventQueue.push(event);
    }
    else
    {
        Event *workCompleted = new Event("WorkCompleted", simulationClock.cycle + instruction->executionTime * 10, this, HIGH);
        masterEventQueue.push(workCompleted);
    }
}
void Execute::process(Event *event)
{
    if (event->type == "WorkCompleted")
    {
        event->handled = true;
        DecodedInstruction *instruction = (DecodedInstruction *)this->staged();

        Store *store = dynamic_cast<Store *>(instruction);
        if (store != NULL)
        {
            INFO << "Execute passing along instruction: " << instruction << "\n";
            this->next->stage(instruction);
        }
        else
        {
            INFO << "Execute executing instruction: " << instruction << "\n";
            instruction->execute(this->cpu);
            delete this->activeRequest;
            this->activeRequest = NULL;

            // Decoded instruction use complete. No further reference to it will be created
            delete instruction;
        }

        this->cpu->instructionsProcessed++;
    }
    else if (event->type == "MemoryRequest")
    {
        event->handled = true;
        Load *load = (Load *)this->staged();
        assert(this->activeRequest == NULL);
        this->activeRequest = new MemoryRequest(load->memoryAddress(this->cpu), this);
        bool accepted = this->cpu->memory->request(this->activeRequest);
        if (!accepted)
        {
            delete this->activeRequest;
            this->activeRequest = NULL;
            Event *event = new Event("MemoryRequest", simulationClock.cycle + 5, this);
            masterEventQueue.push(event);
        }
    }
    else if (event->type == "MemoryReadReady")
    {
        event->handled = true;
        Event *event = new Event("WorkCompleted", simulationClock.cycle, this, HIGH);
        masterEventQueue.push(event);
    }

    Pipeline::process(event);
}
