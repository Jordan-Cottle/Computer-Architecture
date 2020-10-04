/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#include <iostream>

#include "pipeline.h"

PipelineInsertEvent::PipelineInsertEvent(int time, Instruction *instruction, Pipeline *pipeline) : Event("PipelineInsertEvent", time, pipeline), instruction(instruction)
{
}

PipelineCompleteEvent::PipelineCompleteEvent(int time, Pipeline *pipeline) : Event("PipelineCompleteEvent", time, pipeline)
{
}

PipelineFlushEvent::PipelineFlushEvent(int time, Pipeline *pipeline) : Event("PipelineFlushEvent", time, pipeline)
{
}

Pipeline::Pipeline(std::string type) : SimulationDevice(type),
                                       memory(Register<Instruction *>(1))
{
    this->next = NULL;
    this->processingTime = 0;
}

Pipeline::Pipeline(std::string type, Pipeline *next) : SimulationDevice(type),
                                                       memory(Register<Instruction *>(1))
{
    this->next = next;
    this->processingTime = 0;
}

bool Pipeline::free()
{
    return this->memory.read(0) == NULL;
}

void Pipeline::stage(Instruction *instruction)
{
    if (!this->free())
    {
        std::cout << "WARNING: " << this << "is not free!!\n";
        return;
    }

    this->memory.write(0, instruction);
}

void Pipeline::flush()
{
    this->memory.clear(0);
    this->processingTime = 0;
}

Instruction *Pipeline::staged()
{
    return this->memory.read(0);
}

void Pipeline::tick()
{
    if (this->free())
    {
        return;
    }

    // This is just for display purposes
    if (this->processingTime > 1)
    {
        this->processingTime -= 1;
        return;
    }
}

void Pipeline::process(Event *event, EventQueue *eventQueue)
{
    this->eventsProcessed += 1;

    std::cout << this->type << " processing " << event->type << "\n";
    if (event->type == "PipelineInsertEvent")
    {
        PipelineInsertEvent *insert = dynamic_cast<PipelineInsertEvent *>(event);
        Instruction *instruction = insert->instruction;

        // TODO: determine execution time properly
        int executionTime = 2;
        this->processingTime = executionTime;

        this->stage(instruction);

        int scheduled_for = event->time + executionTime;
        if (this->next != NULL)
        {
            // Complete this instruction at specified time
            eventQueue->push(new PipelineCompleteEvent(scheduled_for, this));

            // Send this instruction to downstream pipeline stage at specified time
            eventQueue->push(new PipelineInsertEvent(scheduled_for, instruction, this->next));

            // scheduled_for += 1;
            // TODO: Remove this once proper instruction dispatching is working
            PipelineInsertEvent *new_event = new PipelineInsertEvent(scheduled_for, NULL, this);
            new_event->instruction = new Instruction(instruction->operation, {str(new_event->id), str(scheduled_for)});
            eventQueue->push(new_event);
        }
    }
    else if (event->type == "PipelineCompleteEvent")
    {
        Instruction *instruction = this->staged();
        this->flush();
        if (this->next == NULL)
        {
            delete instruction;
        }
    }
    else if (event->type == "PipelineFlushEvent")
    {
        // TODO: delete instruction pointer?
        this->flush();
    }
    else
    {
        throw UnrecognizedEvent(event->type);
    }
    delete event;
}

std::string Pipeline::__str__()
{
    std::string s = this->type + " (" + str(this->processingTime) + "):\n\t" + addIndent(str(this->memory));

    if (this->next != NULL)
    {
        s += "\n\tNext: " + this->next->type + " (" + str(this->next->processingTime) + ")";
    }

    return s;
}
