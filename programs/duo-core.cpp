#include "test.h"
using namespace Simulation;

int main()
{
    const int ARRAY_A_START = 0x400;
    const int ARRAY_B_START = 0x800;
    const int ARRAY_C_START = 0xC00;
    const int ARRAY_D_START = 0x1000;
    const int ARRAY_SIZE = (ARRAY_B_START - ARRAY_A_START) / sizeof(float);

    const int STACK0_START = 0x2ff;
    const int STACK1_START = 0x3ff;

    std::cout << "Array length: " << ARRAY_SIZE << "\n";

    // Arrays for implementing/testing CPU0.s
    float ARRAY_A[ARRAY_SIZE];
    float ARRAY_B[ARRAY_SIZE];
    float ARRAY_C[ARRAY_SIZE];
    float ARRAY_D[ARRAY_SIZE];

    Memory *ram = new Memory(0x1400, MEMORY_DELAY, {0x100, 0x200, 0x1400});

    // Initialize arrays in fp memory
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = (float)i;
        float b = (float)-i;
        ram->write(ARRAY_A_START + memOffset, a);
        ram->write(ARRAY_B_START + memOffset, b);

        // Initialize arrays for implementing/testing CPU0.s
        ARRAY_A[i] = a;
        ARRAY_B[i] = b;
    }

    cpu0.addPipeline(new Fetch(&cpu0))
        ->addPipeline(new Decode(&cpu0))
        ->addPipeline(new Execute(&cpu0))
        ->addPipeline(new StorePipeline(&cpu0));

    cpu1.addPipeline(new Fetch(&cpu1))
        ->addPipeline(new Decode(&cpu1))
        ->addPipeline(new Execute(&cpu1))
        ->addPipeline(new StorePipeline(&cpu1));

    cpu0.loadProgram("CPU0.bin");
    cpu1.loadProgram("CPU1.bin", 0x100);

    for (int i = 0; i < 0x200; i += 4)
    {
        uint32_t data = ram->read<uint32_t>(i);
        if (data != 0)
        {
            std::cout << i << ": " << RawInstruction(data).keyword() << "\n";
        }
        else
        {
            std::cout << i << ": " << data << "\n";
        }
    }

    cpu0.intRegister.write(14, STACK0_START); // Set stack pointer at bottom of stack
    cpu1.intRegister.write(14, STACK1_START); // Set stack pointer at bottom of stack

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, &cpu0));
    masterEventQueue.push(new Event("Tick", 0, &cpu1));

    while (!cpu0.complete || !cpu1.complete)
    {
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    std::cout << "Analyzing memory state\n";
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        int memOffset = i * sizeof(float);
        float a = ram->read<float>(ARRAY_A_START + memOffset);
        float b = ram->read<float>(ARRAY_B_START + memOffset);
        float c = ram->read<float>(ARRAY_C_START + memOffset);
        float d = ram->read<float>(ARRAY_D_START + memOffset);

        std::cout << str(a) << " + " << str(b) << " = " << str(c) << "\n";
        std::cout << str(a) << " - " << str(b) << " = " << str(d) << "\n";

        // CPU0.s functionality
        ARRAY_C[i] = ARRAY_A[i] + ARRAY_B[i];
        // CPU1.s functionality
        ARRAY_D[i] = ARRAY_A[i] - ARRAY_B[i];

        // Ensure simulation and implementation match
        assert(ARRAY_C[i] == c);
        assert(ARRAY_D[i] == d);
    }
    std::cout << "Memory analysis complete\n";

    std::cout << "Cpu1 clock cycles: " << cpu0.clocksProcessed << "\n";
    std::cout << "Cpu1 instructions processed: " << cpu0.instructionsProcessed << "\n";
    std::cout << "Cpu1 cpi: " << cpu0.cpi() << "\n";
    std::cout << "Cpu2 clock cycles: " << cpu1.clocksProcessed << "\n";
    std::cout << "Cpu2 instructions processed: " << cpu1.instructionsProcessed << "\n";
    std::cout << "Cpu2 cpi: " << cpu1.cpi() << "\n";
}
