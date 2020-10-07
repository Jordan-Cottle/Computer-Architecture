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

EventQueue meq;

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
    Register<Instruction *> instructionMemory = Register<Instruction *>(5);

    TestPipeline testPipeline;

    Fetch fetchUnit = Fetch(&instructionMemory);
    fetchUnit.next = &testPipeline;

    instructionMemory.write(0, new Instruction("ADD", {0, 1, 2}));
    instructionMemory.write(1, new Instruction("SUB", {1, 2, 1}));
    instructionMemory.write(2, new Instruction("MULT", {2, 3, 4}));
    instructionMemory.write(3, new Instruction("DIV", {3, 6, 3}));
    instructionMemory.write(4, new Instruction("BRANCH", {4, 1}));

    std::cout << "Instructions " << instructionMemory << "\n";

    for (int i = 0; i < instructionMemory.size; i++)
    {
        FetchEvent *event = new FetchEvent(i, &fetchUnit, i);
        std::cout << event << "\n";

        meq.push(event);
    }

    std::cout << meq << "\n";

    Clock clock;

    while (clock.cycle < 10)
    {
        std::cout << clock << "\n";
        while (!meq.empty() && meq.nextTime() == clock.cycle)
        {
            Event *event = meq.pop();
            SimulationDevice *device = event->device;

            std::cout << "Processing " << event << "\n";
            device->process(event, &meq);
            std::cout << device << "\n";
        }

        std::cout << "Ticking devices:\n";

        fetchUnit.tick(clock.cycle, &meq);
        decodeUnit.tick(clock.cycle, &meq);

        std::cout << fetchUnit << "\n";
        std::cout << decodeUnit << "\n";

        clock.tick();
    }
}

void programTest()
{
    Program program = Program({new Instruction("fld", {0, 1}),
                               new Instruction("addi", {1, 1, -8}),
                               new Instruction("fadd.d", {4, 0, 2}),
                               new Instruction("stall", {}),
                               new Instruction("stall", {}),
                               new Instruction("fsd", {4, 1}),
                               new Branch("bne", {1, 2}, "Loop")},
                              {{"Loop", 0}, {"Test", 5}});

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
    Register<int> intRegister = Register<int>(1);

    Register<double> fpRegister = Register<double>(2);
    Register<double> fpMemory = Register<double>(2);

    fpMemory.write(0, 3.141592654);   // Pi
    fpRegister.write(1, 2.718281828); // E

    intRegister.write(0, 0); // Read/write from fpMemory[0]

    std::cout << "Load test\n";
    std::cout << fpRegister << "\n";
    Instruction *l = new Instruction("fld", {0, 0});
    Load<double> load = Load<double>(l, &intRegister);

    load.execute(&fpRegister, &fpMemory);
    std::cout << fpRegister << "\n";

    std::cout << "Add immediate test\n";
    std::cout << intRegister << "\n";
    Instruction *a = new Instruction("addi", {0, 0, 1});
    Add<int> add = Add<int>(a, a->arguments[2]);
    add.execute(&intRegister);
    std::cout << intRegister << "\n";

    std::cout << "Add test\n";
    std::cout << fpRegister << "\n";
    Instruction *fa = new Instruction("fadd.d", {1, 0, 1});
    Add<double> fadd = Add<double>(fa);
    fadd.execute(&fpRegister);
    std::cout << fpRegister << "\n";

    std::cout << "Store test\n";
    std::cout << fpMemory << "\n";
    Instruction *s = new Instruction("fsd", {1, 0});
    Store<double> store = Store<double>(s, &intRegister);

    store.execute(&fpRegister, &fpMemory);
    std::cout << fpMemory << "\n";
}

int main()
{
    fpTest();
    return 0;
}