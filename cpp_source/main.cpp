#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include "event_queue.h"
#include "event.h"

#include "instruction.h"

#include "device.h"

#include "sim_register.h"
#include "pipeline.h"
#include "fetch.h"
#include "decode.h"

#include "memory_instruction.h"
#include "arithmetic_instruction.h"

#include "cpu.h"

#include "execute.h"
#include "store.h"

#include "control_instructions.h"

#include "sim_memory.h"

#include "opcodes.h"

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

TestPipeline testPipeline;

void fetchTest()
{
    Fetch fetchUnit = Fetch(&cpu);
    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    fetchUnit.process(new Event("Fetch", 0, &fetchUnit));
    assert(fetchUnit.staged() != NULL);
    RawInstruction *instruction = fetchUnit.staged();

    fetchUnit.tick();
    assert(testPipeline.staged() != NULL);
    assert(testPipeline.staged() == instruction);

    testPipeline.tick();
}

constexpr float PI = 3.141592654f;
constexpr float E = 2.718281828f;

void fpTest()
{
    const int RAM_LOCATION = 124;
    cpu.ram.write(RAM_LOCATION, PI);
    cpu.fpRegister.write(2, E);

    // Read/write from ram[124] (offsets in program are set to +/- 4)
    cpu.intRegister.write(1, RAM_LOCATION - 4);

    Fetch fetchUnit = Fetch(&cpu);
    cpu.addPipeline(&fetchUnit);

    cpu.loadProgram("fpTest.bin");

    RawInstruction instruction = RawInstruction(cpu.ram.read<uint32_t>(0));
    Load load = Load(&instruction);

    assert(cpu.fpRegister.read(1) == 0);
    load.execute(&cpu);
    assert(cpu.fpRegister.read(1) == PI);

    instruction = RawInstruction(cpu.ram.read<uint32_t>(4));
    Add add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION - 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);

    instruction = RawInstruction(cpu.ram.read<uint32_t>(8));
    add = Add(&instruction);

    assert(cpu.fpRegister.read(1) == PI);
    assert(cpu.fpRegister.read(2) == E);
    add.execute(&cpu);
    assert(cpu.fpRegister.read(3) - (PI + E) < 0.000001);

    instruction = RawInstruction(cpu.ram.read<uint32_t>(12));
    Store store = Store(&instruction);

    assert(cpu.ram.read<float>(RAM_LOCATION) == PI);
    store.execute(&cpu);
    assert(cpu.ram.read<float>(RAM_LOCATION) == cpu.fpRegister.read(3));

    instruction = RawInstruction(cpu.ram.read<uint32_t>(16));
    add = Add(&instruction);

    assert(cpu.intRegister.read(1) == RAM_LOCATION + 4);
    add.execute(&cpu);
    assert(cpu.intRegister.read(1) == RAM_LOCATION);
}

void decodeTest()
{

    Decode decode = Decode(&cpu);

    cpu.addPipeline(&decode);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.ram.read<uint32_t>(0));

    decode.stage(instruction);
    decode.tick();

    // Check that instruction made it to next stage
    assert(testPipeline.staged() != NULL);

    // Check instruction was properly decoded into a Load
    Load *load = dynamic_cast<Load *>(testPipeline.staged());
    assert(load != NULL);
    assert(load->isFp);
    assert(load->keyword() == "flw");
    testPipeline.tick();
}

void executeTest()
{
    Execute execute = Execute(&cpu);
    cpu.addPipeline(&execute);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.ram.read<uint32_t>(4));

    Add *add = new Add(instruction);

    cpu.intRegister.write(1, 0);
    assert(cpu.intRegister.read(1) == 0);
    execute.stage(add);
    execute.tick();
    testPipeline.tick();

    assert(cpu.intRegister.read(1) == 8);
}

void storeTest()
{

    const int RAM_LOCATION = 100;
    cpu.intRegister.write(1, RAM_LOCATION);

    cpu.fpRegister.write(3, PI); // Pi

    StorePipeline store = StorePipeline(&cpu);
    cpu.addPipeline(&store);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.ram.read<uint32_t>(12));
    Store *storeInstruction = new Store(instruction);

    assert(cpu.ram.read<float>(RAM_LOCATION - 4) == 0);
    store.stage(storeInstruction);
    store.tick();
    assert(cpu.ram.read<float>(RAM_LOCATION - 4) == PI);
}

void cpuTest()
{
    constexpr int ASM_I = 1;
    constexpr int ASM_END = 2;
    const float INITIAL = 1.0;
    const float OFFSET = 0.5;

    const int ARRAY_SIZE = 10;
    const int ARRAY_START = 64;

    const float VALUE_ADDED = 1.0;

    // Indexes of array
    cpu.intRegister.write(ASM_I, ARRAY_START + (ARRAY_SIZE - 1) * sizeof(INITIAL)); // -1 for 0 indexed arrays

    // Since we're counting down and using != as the branch we need the end to be one less than first index
    cpu.intRegister.write(ASM_END, ARRAY_START - 4);

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

    cpu.loadProgram("test_program.bin");

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, &cpu));

    while (!cpu.complete)
    {
        // std::cout << "\n"
        //           << simulationClock << "\n";

        // std::cout << "\n~~~EventQueue~~~\n";
        // std::cout << masterEventQueue << "\n";

        // std::cout << "\n~~~Processing events~~~\n";
        masterEventQueue.tick(simulationClock.cycle);

        // std::cout << "\n~~~Ticking cpu~~~\n";

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        float expected = (INITIAL + i * OFFSET) + VALUE_ADDED;
        float actual = cpu.ram.read<float>(ARRAY_START + (i * sizeof(INITIAL)));
        // std::cout << "Expected: " << expected << "\n";
        // std::cout << "Actual: " << actual << "\n";
        assert(actual == expected);
    }

    std::cout << "Memory state verified!\n";
    // std::cout << cpu.ram << "\n";
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
}

void binaryReadTest()
{
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    std::vector<uint32_t> expected = {
        0b00000000000000001010000000000111,
        0b00000000000000000000000000110011,
        0b00000000001000000000000111010011,
        0b00000000000000000000000000110011,
        0b00000000000000000000000000110011,
        0b00000000001100001010000000100111,
        0b11111111110000001000000010010011,
        0b00000000001000001001000001100011,
    };

    for (uint32_t i = 0; i < expected.size(); i++)
    {
        uint32_t instruction = cpu.ram.read<uint32_t>(i * 4);
        RawInstruction rInstruction = RawInstruction(instruction);
        assert(instruction == expected[i]);
    }
}

void testOpcodes()
{
    assert(getImmediateS(0xfe000f80) == 0xFFF);

    for (int i = 0; i < 32; i++)
    {
        assert(setBit(0, i, 1) == 1u << i);
    }

    assert(getImmediateSB(0xfe000f80) == (0xFFF << 1));

    assert(getImmediateU(0xABCDEF12) == 0xABCDE000);
    assert(getImmediateUB(0xABCDEF12) == 0x001DE2BC);

    assert(sign_extend(1024, 11) == 1024);
    assert((int)sign_extend(1, 0) == -1);
    assert(sign_extend(2048, 11) == 0xFFFFF800);
    assert(sign_extend(2048, 11) + (2048 << 1) == 0x00000800);
    assert((int)sign_extend(4, 2) == -4);
    assert((int)sign_extend(-4, 2) == -4);
}

void runProgram(std::string name)
{
    const int ARRAY_A_START = 0x400;
    const int ARRAY_B_START = 0x800;
    const int ARRAY_C_START = 0xC00;
    const int ARRAY_SIZE = (ARRAY_B_START - ARRAY_A_START) / sizeof(float);

    std::cout << "Array length: " << ARRAY_SIZE << "\n";

    // Arrays for implementing/testing CPU0.s
    float ARRAY_A[ARRAY_SIZE];
    float ARRAY_B[ARRAY_SIZE];
    float ARRAY_C[ARRAY_SIZE];

    // Initialize arrays in fp memory
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = rand();
        float b = rand();
        cpu.ram.write(ARRAY_A_START + memOffset, a);
        cpu.ram.write(ARRAY_B_START + memOffset, b);

        // Initialize arrays for implementing/testing CPU0.s
        ARRAY_A[i] = a;
        ARRAY_B[i] = b;
    }

    cpu.addPipeline(new Fetch(&cpu))
        ->addPipeline(new Decode(&cpu))
        ->addPipeline(new Execute(&cpu))
        ->addPipeline(new StorePipeline(&cpu));

    cpu.loadProgram(name);

    cpu.intRegister.write(14, 0x2FF); // Set stack pointer at bottom of stack

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, &cpu));

    while (!cpu.complete)
    {
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = cpu.ram.read<float>(ARRAY_A_START + memOffset);
        float b = cpu.ram.read<float>(ARRAY_B_START + memOffset);
        float c = cpu.ram.read<float>(ARRAY_C_START + memOffset);

        std::cout << str(a) << " + " << str(b) << " = " << str(c) << "\n";

        // CPU0.s functionality
        ARRAY_C[i] = ARRAY_A[i] + ARRAY_B[i];

        // Ensure simulation and implementation match
        assert(ARRAY_C[i] == c);
    }

    std::cout << "Cpu clock cycles: " << cpu.clocksProcessed << "\n";
    std::cout << "Cpu instructions processed: " << cpu.instructionsProcessed << "\n";
    std::cout << "Cpu cpi: " << cpu.cpi() << "\n";
}

void run_tests()
{
    // These won't use the meq and so won't conflict with each other
    std::cout << "Running tests\n";
    fpTest();
    testOpcodes();
    memoryTest();
    binaryReadTest();
    fetchTest();
    decodeTest();
    executeTest();
    storeTest();
    std::cout << "Tests completed\n";
}

int main()
{
    // run_tests();
    runProgram("CPU0.bin");
    return 0;
}
