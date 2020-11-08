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
Fetch fetchUnit = Fetch(&cpu);

void fetchTest()
{
    cpu.addPipeline(&fetchUnit);
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    masterEventQueue.tick(0);
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

    assert(cpu.intRegister.read(1) == 0);
    execute.stage(add);
    execute.tick();
    testPipeline.tick();

    std::cout << cpu.intRegister << "\n";
    assert(cpu.intRegister.read(1) == 4);
}

void storeTest()
{

    cpu.intRegister.write(1, 20); // Store in memory address 20

    cpu.fpRegister.write(3, PI); // Pi
    std::cout << "Float " << cpu.fpRegister << "\n";

    StorePipeline store = StorePipeline(&cpu);
    cpu.addPipeline(&store);

    cpu.loadProgram("fpTest.bin");

    RawInstruction *instruction = new RawInstruction(cpu.ram.read<uint32_t>(12));
    Store *storeInstruction = new Store(instruction);

    assert(cpu.ram.read<float>(16) == 0);
    store.stage(storeInstruction);
    store.tick();
    assert(cpu.ram.read<float>(16) == PI);
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
        0b00000000000000000000000000011011,
        0b00000000001100000000000100010111,
        0b00000000000000000000000000011011,
        0b00000000000000000000000000011011,
        0b00000000001100001010000001001011,
        0b11111111110000001000000010010011,
        0b00000000000100010100000000001111,
    };

    for (uint32_t i = 0; i < expected.size(); i++)
    {
        uint32_t instruction = cpu.ram.read<uint32_t>(i * 4);
        RawInstruction rInstruction = RawInstruction(instruction);
        std::cout << rInstruction << "\n";
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
}

void runProgram(std::string name)
{
    const int ARRAY_SIZE = 0x400;
    const int ARRAY_A_START = 0x400;
    const int ARRAY_B_START = 0x800;
    const int ARRAY_C_START = 0xC00;

    // Initialize arrays in fp memory
    for (int i = 0; i < ARRAY_SIZE; i += sizeof(float))
    {
        cpu.ram.write(ARRAY_A_START + i, rand());
        cpu.ram.write(ARRAY_B_START + i, rand());
    }

    cpu.addPipeline(new Fetch(&cpu))
        ->addPipeline(new Decode(&cpu))
        ->addPipeline(new Execute(&cpu))
        ->addPipeline(new StorePipeline(&cpu));

    cpu.loadProgram(name);

    cpu.intRegister.write(14, 0x2FF); // Set stack pointer at bottom of stack
    std::cout << cpu.ram << "\n";

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

    std::cout << cpu.ram << "\n";
}

int main()
{
    fpTest();
    return 0;
}
