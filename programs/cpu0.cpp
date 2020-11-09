#include "test.h"
using namespace Simulation;

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
        cpu.memory.write(ARRAY_A_START + memOffset, a);
        cpu.memory.write(ARRAY_B_START + memOffset, b);

        // Initialize arrays for implementing/testing CPU0.s
        ARRAY_A[i] = a;
        ARRAY_B[i] = b;
    }

    cpu.addPipeline(&fetchUnit)
        ->addPipeline(&decodeUnit)
        ->addPipeline(&executeUnit)
        ->addPipeline(&storeUnit);

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

    std::cout << "Analyzing memory state\n";
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = cpu.memory.read<float>(ARRAY_A_START + memOffset);
        float b = cpu.memory.read<float>(ARRAY_B_START + memOffset);
        float c = cpu.memory.read<float>(ARRAY_C_START + memOffset);

        std::cout << str(a) << " + " << str(b) << " = " << str(c) << "\n";

        // CPU0.s functionality
        ARRAY_C[i] = ARRAY_A[i] + ARRAY_B[i];

        // Ensure simulation and implementation match
        assert(ARRAY_C[i] == c);
    }
    std::cout << "Memory analysis complete\n";

    std::cout << "Cpu clock cycles: " << cpu.clocksProcessed << "\n";
    std::cout << "Cpu instructions processed: " << cpu.instructionsProcessed << "\n";
    std::cout << "Cpu cpi: " << cpu.cpi() << "\n";
}

int main()
{
    runProgram("CPU0.bin");
    return 0;
}
