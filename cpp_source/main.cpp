#include <iostream>
#include <stdlib.h>
#include <assert.h>

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

#include "control_instructions.h"

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

    cpu.loadProgram(new Program({new Instruction("ADD", {0, 1, 2}),
                                 new Instruction("SUB", {1, 2, 1}),
                                 new Instruction("MULT", {2, 3, 4}),
                                 new Instruction("DIV", {3, 6, 3}),
                                 new Branch("BRANCH", {}, "Test")},
                                {{"Test", 0}}));
    std::cout << cpu.program << "\n";

    // Initialize cpu
    cpu.tick(-1, &meq);

    Clock clock;
    while (clock.cycle <= 10)
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);

        std::cout << "Ticking devices:\n";

        cpu.tick(clock.cycle, &meq);
        std::cout << fetchUnit << "\n";
        clock.tick();
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

    cpu.fpMemory.write(1, 3.14);
    instruction = new Instruction("fsd", {0, 0});
    Store store = Store(instruction, &cpu.intRegister);
    event = new PipelineInsertEvent(1, &execute, &store);
    meq.push(event);

    Branch *branchInstruction = new Branch("branch", {}, "Test");
    DecodedBranch branch = DecodedBranch(branchInstruction, 42);
    event = new PipelineInsertEvent(2, &execute, &branch);
    meq.push(event);

    std::cout << cpu.intRegister << "\n";

    Clock clock;
    while (!meq.empty())
    {
        std::cout << clock << "\n";
        meq.tick(clock.cycle);

        cpu.programCounter = clock.cycle;
        cpu.tick(clock.cycle, &meq);

        clock.tick();
    }

    std::cout << "Integer ";
    std::cout << cpu.intRegister << "\n";
    assert(cpu.intRegister.read(0) == 1);

    std::cout << "fpMemory[1]: " << cpu.fpMemory.read(1) << "\n";
    assert(cpu.fpMemory.read(1) == 3.14);

    std::cout << "Final PC: " << cpu.programCounter << "\n";
    assert(cpu.programCounter == 42);
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

    const double INITIAL = 0.5;
    const double OFFSET = 0.5;

    // Indexes of array
    cpu.intRegister.write(START, 100);
    cpu.intRegister.write(END, 0);

    // Constant float to add to fp array
    cpu.fpRegister.write(2, 1.0);

    // Initialize array in fp memory
    for (int i = 0; i < 100; i++)
    {
        cpu.fpMemory.write(i + 1, INITIAL + i * OFFSET);
    }

    cpu.addPipeline(new Fetch(&cpu))
        ->addPipeline(new Decode(&cpu))
        ->addPipeline(new Execute(&cpu))
        ->addPipeline(new StorePipeline(&cpu));

    cpu.loadProgram(&program);

    std::cout << "Initial float memory " << cpu.fpMemory << "\n";

    std::cout << "Instruction " << cpu.program << "\n";

    // Set up initial fetch event (so meq isn't empty)
    meq.push(new FetchEvent(0, (Fetch *)cpu.pipelines[0]));

    Clock clock;
    while (cpu.programCounter != 9)
    {
        std::cout << "\n"
                  << clock << "\n";

        std::cout << "~~~Processing events~~~\n";
        meq.tick(clock.cycle);

        std::cout << "\n~~~Ticking cpu~~~\n";
        cpu.tick(clock.cycle, &meq);

        std::cout << cpu.intRegister << "\n";

        clock.tick();
    }
    std::cout << "Program complete!\n";

    std::cout << "\n~~~Result~~~\n";
    std::cout << "Float Memory " << cpu.fpMemory << "\n";

    for (int i = 0; i < 100; i++)
    {
        assert(cpu.fpMemory.read(i + 1) == 1.0 + INITIAL + i * OFFSET);
    }
}

int main()
{
    cpuTest();
    return 0;
}
