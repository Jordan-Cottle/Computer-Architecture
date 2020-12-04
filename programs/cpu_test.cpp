#include "test.h"
using namespace Simulation;

int main()
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
        float data = INITIAL + i * OFFSET;
        cpu.memory->write(ARRAY_START + (i * sizeof(INITIAL)), MFMT(data));
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
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }

    std::cout << "Program complete!\n";

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        float expected = (INITIAL + i * OFFSET) + VALUE_ADDED;
        float actual = cpu.memory->readFloat(ARRAY_START + (i * sizeof(INITIAL)));
        OUT << "Expected: " << expected << "\n";
        OUT << "Actual: " << actual << "\n";
        assert(actual == expected);
    }

    std::cout << "Memory state verified!\n";

    return 0;
}