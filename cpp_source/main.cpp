#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include "event_queue.h"
#include "event.h"

#include "instruction.h"
#include "instruction_queue.h"

#include "device.h"

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

#include "sim_memory.h"

#include "simulation.h"
using namespace Simulation;

// Echo any events/instructions for debugging partial pipelines
struct TestPipeline : Pipeline
{
    TestPipeline() : Pipeline("TestPipeline")
    {
    }

    void tick()
    {
        RawInstruction *staged = this->staged();

        std::cout << this->type << " T " << simulationClock.cycle << ": ";
        if (staged == NULL)
        {
            std::cout << " no instruction\n";
        }
        else
        {
            std::cout << this->staged() << "\n";
        }

        Pipeline::tick();
    }
};

#define ASM_I 1
#define END 2
Program program = Program({
                              new Instruction("flw", {0, ASM_I}),
                              new Instruction("stall", {}),
                              new Instruction("fadd.s", {3, 0, 2}),
                              new Instruction("stall", {}),
                              new Instruction("stall", {}),
                              new Instruction("fsw", {3, ASM_I}),
                              new Instruction("addi", {ASM_I, ASM_I, -4}),
                              new Branch("bne", {ASM_I, END}, "Loop"),
                              new Instruction("halt", {}),
                          },
                          {{"Loop", 0}});

TestPipeline testPipeline;

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

    // Set up initial fetch event (so masterEventQueue isn't empty)
    masterEventQueue.push(new Event(0, (Fetch *)cpu.pipelines[0]));

    while (simulationClock.cycle <= 10)
    {
        std::cout << simulationClock << "\n";
        masterEventQueue.tick(simulationClock.cycle);

        std::cout << "Ticking devices:\n";

        cpu.tick();
        std::cout << fetchUnit << "\n";
        simulationClock.tick();
    }

    delete cpu.program;
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

constexpr float PI = 3.141592654f;
constexpr float E = 2.718281828f;

void fpTest()
{
    cpu.ram.write(0, PI);
    cpu.fpRegister.write(1, E);

    cpu.intRegister.write(0, 0); // Read/write from ram[0]

    Instruction *l = new Instruction("flw", {0, 0});
    Load load = Load(l, &cpu.intRegister);

    assert(cpu.fpRegister.read(0) == 0);
    load.execute(&cpu);
    assert(cpu.fpRegister.read(0) == PI);

    Instruction *a = new Instruction("addi", {0, 0, 4});
    Add add = Add(a, a->arguments[2]);

    assert(cpu.intRegister.read(0) == 0);
    add.execute(&cpu);
    assert(cpu.intRegister.read(0) == 4);

    Instruction *fa = new Instruction("fadd.s", {1, 0, 1});
    Add fadd = Add(fa);

    assert(cpu.fpRegister.read(1) == E);
    fadd.execute(&cpu);
    assert(cpu.fpRegister.read(1) == PI + E);

    Instruction *s = new Instruction("fsw", {1, 0});
    Store store = Store(s, &cpu.intRegister);

    assert(cpu.ram.read<float>(4) == 0);
    store.execute(&cpu);
    assert(cpu.ram.read<float>(4) == PI + E);

    std::cout << cpu.ram << "\n";
}

void decodeTest()
{

    Instruction *instruction = new Instruction("fsw", {0, 0});

    Decode decode = Decode(&cpu);

    cpu.addPipeline(&decode);
    cpu.addPipeline(&testPipeline);

    cpu.intRegister.write(0, 2);

    decode.stage(instruction);
    decode.tick();
    testPipeline.tick();
}

void executeTest()
{
    Execute execute = Execute(&cpu);
    cpu.addPipeline(&execute);
    cpu.addPipeline(&testPipeline);

    Instruction *instruction = new Instruction("addi", {0, 0});
    Add *add = new Add(instruction, 4);

    execute.stage(add);
    execute.tick();
    testPipeline.tick();

    std::cout << cpu.intRegister << "\n";
    assert(cpu.intRegister.read(0) == 4);

    instruction = new Instruction("fsw", {0, 0});
    Store *store = new Store(instruction, &cpu.intRegister);

    assert(testPipeline.staged() == NULL);
    execute.stage(store);
    execute.tick();
    assert(testPipeline.staged() != NULL);

    testPipeline.tick();

    Branch *branchInstruction = new Branch("branch", {}, "Test");
    BranchInstruction *branch = new BranchInstruction(branchInstruction, 42);

    execute.stage(branch);
    execute.tick();
    testPipeline.tick();

    std::cout << cpu.programCounter << "\n";
    assert(cpu.programCounter.value == 42);
}

void storeTest()
{

    cpu.intRegister.write(0, 0); // Store in memory address 0
    cpu.intRegister.write(1, 4); // Store in memory address 1

    cpu.fpRegister.write(0, PI); // Pi
    cpu.fpRegister.write(1, E);  // E
    std::cout << "Float " << cpu.fpRegister << "\n";

    StorePipeline store = StorePipeline(&cpu);
    cpu.addPipeline(&store);

    Instruction *instruction = new Instruction("fsw", {0, 0});
    instruction = new Store(instruction, &cpu.intRegister);

    assert(cpu.ram.read<float>(0) == 0);
    store.stage(instruction);
    store.tick();
    assert(cpu.ram.read<float>(0) == PI);

    instruction = new Instruction("fsw", {1, 1});
    instruction = new Store(instruction, &cpu.intRegister);

    assert(cpu.ram.read<float>(4) == 0);
    store.stage(instruction);
    store.tick();
    assert(cpu.ram.read<float>(4) == E);

    std::cout << cpu.ram << "\n";
}

void cpuTest()
{

    const float INITIAL = 1.0;
    const float OFFSET = 0.5;

    const int ARRAY_SIZE = 10;
    const int ARRAY_START = 20; // Don't make it bigger than Cpu.memorySize - ARRAY_SIZE

    const float VALUE_ADDED = 1.0;

    // Indexes of array
    cpu.intRegister.write(ASM_I, ARRAY_START + (ARRAY_SIZE - 1) * sizeof(INITIAL)); // -1 for 0 indexed arrays

    // Since we're counting down and using != as the branch we need the end to be one less than first index
    cpu.intRegister.write(END, ARRAY_START - 4);

    // Constant float to add to fp array
    cpu.fpRegister.write(2, VALUE_ADDED);

    // Initialize array in fp memory
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        cpu.ram.write(ARRAY_START + (i * sizeof(INITIAL)), INITIAL + i * OFFSET);
    }

    cpu.addPipeline(new Fetch(&cpu))
        ->addPipeline(new Decode(&cpu))
        ->addPipeline(new Execute(&cpu))
        ->addPipeline(new StorePipeline(&cpu));

    cpu.loadProgram(&program);

    std::cout << cpu.program << "\n";

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, &cpu));

    while (!cpu.complete)
    {
        std::cout << "\n"
                  << simulationClock << "\n";

        std::cout << "\n~~~EventQueue~~~\n";
        std::cout << masterEventQueue << "\n";

        std::cout << "\n~~~Processing events~~~\n";
        masterEventQueue.tick(simulationClock.cycle);

        std::cout << "\n~~~Ticking cpu~~~\n";

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    // Uncomment this to see the ram printed out (it's big)
    std::cout << cpu.ram << "\n";

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        float expected = (INITIAL + i * OFFSET) + VALUE_ADDED;
        float actual = cpu.ram.read<float>(ARRAY_START + (i * sizeof(INITIAL)));
        // std::cout << "Expected: " << expected << "\n";
        // std::cout << "Actual: " << actual << "\n";
        assert(actual == expected);
    }

    std::cout << "Memory state verified!\n";

    std::cout << cpu.ram << "\n";
}

void memoryTest()
{
    Memory memory = Memory(8, 20);

    for (int i = 0; i < 32; i++)
    {
        int num = 1 << i;
        memory.write(0, num);
        assert(memory.read<int>(0) == num);
    }

    float num = .1f;
    memory.write(4, num);
    assert(memory.read<float>(4) == num);

    std::cout << memory << "\n";
}

void binaryReadTest()
{
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    std::vector<uint32_t> expected = {
        0b00000000000000000010000011000011,
        0b00000000000000000000000000000011,
        0b00000000001100000000000100010111,
        0b00000000000000000000000000000011,
        0b00000000000000000000000000000011,
        0b00000000001100001010000001001011,
        0b11111111110000001000000010010011,
        0b00000000000100010100000000001111,
    };

    for (uint32_t i = 0; i < expected.size(); i++)
    {
        uint32_t instruction = cpu.ram.read<uint32_t>(i * 4);
        std::cout << instruction << "\n";
        assert(instruction == expected[i]);
    }
}

int main()
{
    cpuTest();
    return 0;
}
