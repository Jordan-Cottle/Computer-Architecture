#include <iostream>
#include <stdlib.h>

#include "event_queue.h"
#include "event.h"

#include "instruction.h"
#include "instruction_queue.h"

#include "device.h"
#include "clock.h"

#include "sim_register.h"
#include "pipeline.h"
#include "fetch.h"
#include "decode.h"

#include "program.h"

#include "memory_instruction.h"
#include "arithmetic_instruction.h"

#include "cpu.h"

#include "execute.h"
#include "store.h"

// Echo any events/instructions for debugging partial pipelines
struct TestPipeline : Pipeline
{
    TestPipeline() : Pipeline("TestPipeline")
    {
    }

    void tick(ulong time, EventQueue *eventQueue)
    {
        Instruction *staged = this->staged();

        std::cout << this->type << " T " << time << ": ";
        if (staged == NULL)
        {
            std::cout << " no instruction\n";
        }
        else
        {
            std::cout << this->staged() << "\n";
        }

        Pipeline::tick(time, eventQueue);
    }
};

EventQueue meq;

#define START 1
#define END 2
Program program = Program({
                              new Instruction("fld", {0, 1}),
                              new Instruction("stall", {}),
                              new Instruction("fadd.d", {3, 0, 2}),
                              new Instruction("stall", {}),
                              new Instruction("stall", {}),
                              new Instruction("addi", {START, START, -1}),
                              new Instruction("fsd", {3, 1}),
                              new Branch("bne", {START, END}, "Loop"),
                          },
                          {{"Loop", 0}});

void instructionQueueTest()
{
    std::vector<Instruction *> instructions = std::vector<Instruction *>();
    for (int i = 0; i < 10; i++)
    {
        instructions.push_back(new Instruction("ADD", {i, i, i + 1}));
    }

    InstructionQueue instructionQueue = InstructionQueue(instructions);

    std::cout << instructionQueue << "\n";

    for (int i = 0; i < 3; i++)
    {
        Instruction *next = instructionQueue.next();

        std::cout << "Processing: " << next << "\n";
        delete next;
    }

    std::cout << instructionQueue << "\n";
}

void memory_test()
{
    Register<Event *> memory = Register<Event *>(2, "EventStorage");

    Event *e = new Event(0, NULL);
    Event *f = new Event(0, NULL);

    memory.write(0, e);
    memory.write(1, f);

    std::cout << memory << "\n\n";
    memory.clear(1);
    std::cout << memory << "\n\n";
    memory.clear();
    std::cout << memory << "\n\n";

    delete e;
    delete f;
}

void fetchTest()
{
    Cpu cpu;

    Fetch fetchUnit = Fetch(&cpu);
    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(new TestPipeline());

    cpu.instructionMemory.write(0, new Instruction("ADD", {0, 1, 2}));
    cpu.instructionMemory.write(1, new Instruction("SUB", {1, 2, 1}));
    cpu.instructionMemory.write(2, new Instruction("MULT", {2, 3, 4}));
    cpu.instructionMemory.write(3, new Instruction("DIV", {3, 6, 3}));
    cpu.instructionMemory.write(4, new Instruction("BRANCH", {4, 1}));

    std::cout << "Instructions " << cpu.instructionMemory << "\n";

    for (int i = 0; i < cpu.instructionMemory.size; i++)
    {
        FetchEvent *event = new FetchEvent(i, &fetchUnit);
        std::cout << event << "\n";

        meq.push(event);
    }

    std::cout << meq << "\n";

    Clock clock;

    while (!meq.empty())
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);

        std::cout << "Ticking devices:\n";

        cpu.tick(clock.cycle, &meq);
        std::cout << fetchUnit << "\n";
        clock.tick();

        // TODO Remove this once branches are handled
        if (cpu.programCounter > 4)
        {
            cpu.programCounter = 0;
        }
    }
}

void programTest()
{
    program.labels["Test"] = 5;
    std::cout << program << "\n\n";

    for (auto label : {"Loop", "Test"})
    {
        int line = program.index(label);
        std::cout << label << ": " << line << "\n";
        std::cout << "Instruction at '" << label << "': " << program.line(line) << "\n";
    }
}

void fpTest()
{
    Cpu cpu;

    cpu.fpMemory.write(0, 3.141592654);   // Pi
    cpu.fpRegister.write(1, 2.718281828); // E

    cpu.intRegister.write(0, 0); // Read/write from fpMemory[0]

    std::cout << "Load test\n";
    std::cout << cpu.fpRegister << "\n";
    Instruction *l = new Instruction("fld", {0, 0});
    Load load = Load(l, &cpu.intRegister);

    load.execute(&cpu);
    std::cout << cpu.fpRegister << "\n";

    std::cout << "Add immediate test\n";
    std::cout << cpu.intRegister << "\n";
    Instruction *a = new Instruction("addi", {0, 0, 1});
    Add add = Add(a, a->arguments[2]);
    add.execute(&cpu);
    std::cout << cpu.intRegister << "\n";

    std::cout << "Add test\n";
    std::cout << cpu.fpRegister << "\n";
    Instruction *fa = new Instruction("fadd.d", {1, 0, 1});
    Add fadd = Add(fa);
    fadd.execute(&cpu);
    std::cout << cpu.fpRegister << "\n";

    std::cout << "Store test\n";
    std::cout << cpu.fpMemory << "\n";
    Instruction *s = new Instruction("fsd", {1, 0});
    Store store = Store(s, &cpu.intRegister);

    store.execute(&cpu);
    std::cout << cpu.fpMemory << "\n";
}

void decodeTest()
{
    Cpu cpu = Cpu();
    Instruction *instruction = new Instruction("fsd", {0, 0});

    Decode decode = Decode(&cpu);

    cpu.addPipeline(&decode);
    cpu.addPipeline(new TestPipeline());

    cpu.intRegister.write(0, 2);

    Event *event = new PipelineInsertEvent(0, &decode, instruction);

    meq.push(event);

    Clock clock;
    while (!meq.empty())
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);
        cpu.tick(clock.cycle, &meq);
        clock.tick();
    }
}

void executeTest()
{
    Cpu cpu = Cpu();

    Execute execute = Execute(&cpu);
    cpu.addPipeline(&execute);
    cpu.addPipeline(new TestPipeline());

    Instruction *instruction = new Instruction("addi", {0, 1});
    Add add = Add(instruction, instruction->arguments[1]);

    Event *event = new PipelineInsertEvent(0, &execute, &add);
    meq.push(event);

    instruction = new Instruction("fsd", {0, 0});
    Store store = Store(instruction, &cpu.intRegister);

    event = new PipelineInsertEvent(1, &execute, &store);
    meq.push(event);

    std::cout << cpu.intRegister << "\n";

    Clock clock;
    while (!meq.empty())
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);
        cpu.tick(clock.cycle, &meq);
        clock.tick();
    }

    std::cout << "Integer ";
    std::cout << cpu.intRegister << "\n";
}

void storeTest()
{
    Cpu cpu = Cpu();
    cpu.intRegister.write(0, 0); // Store in memory address 0
    cpu.intRegister.write(1, 1); // Store in memory address 1

    cpu.fpRegister.write(0, 3.141592654); // Pi
    cpu.fpRegister.write(1, 2.718281828); // E

    StorePipeline store = StorePipeline(&cpu);
    cpu.addPipeline(&store);

    Instruction *instruction = new Instruction("fsd", {0, 0});
    instruction = new Store(instruction, &cpu.intRegister);

    Event *event = new PipelineInsertEvent(0, &store, instruction);
    meq.push(event);

    instruction = new Instruction("fsd", {1, 1});
    instruction = new Store(instruction, &cpu.intRegister);

    event = new PipelineInsertEvent(1, &store, instruction);
    meq.push(event);

    std::cout << "Float " << cpu.fpRegister << "\n";

    std::cout << "Float Memory " << cpu.fpMemory << "\n";

    Clock clock;
    while (!meq.empty())
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);
        cpu.tick(clock.cycle, &meq);
        clock.tick();
    }

    std::cout << "Float memory ";
    std::cout << cpu.fpMemory << "\n";
}

void cpuTest()
{
    Cpu cpu = Cpu();

    // Indexes of array
    cpu.intRegister.write(START, 100);
    cpu.intRegister.write(END, 0);

    // Constant float to add to fp array
    cpu.fpRegister.write(2, 1.0);

    // Initialize array in fp memory
    for (int i = 0; i < 100; i++)
    {
        cpu.fpMemory.write(i + 1, 0.5 + (i * 0.5));
    }

    cpu.addPipeline(new Fetch(&cpu))
        ->addPipeline(new Decode(&cpu))
        ->addPipeline(new Execute(&cpu))
        ->addPipeline(new StorePipeline(&cpu));

    cpu.loadProgram(&program);

    int branchPos = -1;
    for (auto instruction : program.instructions)
    {
        Branch *branch = dynamic_cast<Branch *>(instruction);

        if (branch != NULL)
        {
            break;
        }
        else
        {
            branchPos += 1;
        }
    }

    std::cout << "Initial float memory " << cpu.fpMemory << "\n";

    std::cout << "Instruction " << cpu.instructionMemory << "\n";

    // Set up initial fetch event (so meq isn't empty)
    meq.push(new FetchEvent(0, (Fetch *)cpu.pipelines[0]));

    Clock clock;
    while (!meq.empty())
    {
        std::cout << "\n"
                  << clock << "\n";

        std::cout << "~~~Processing events~~~\n";
        meq.tick(clock.cycle);

        std::cout << "\n~~~Ticking cpu~~~\n";
        cpu.tick(clock.cycle, &meq);

        std::cout << cpu.intRegister << "\n";
        if (cpu.programCounter > branchPos)
        {
            if (cpu.intRegister.read(START) == cpu.intRegister.read(END))
            {
                break;
            }
            cpu.programCounter = 0;
        }

        clock.tick();
    }
    std::cout << "Program complete!\n";

    std::cout << "\n~~~Result~~~\n";

    std::cout << "Float Memory " << cpu.fpMemory << "\n";
}

int main()
{
    cpuTest();
    return 0;
}
