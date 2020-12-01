#include "test.h"
using namespace Simulation;

int main()
{
    const int MAT_A_START = 0x400;
    const int MAT_B_START = 0x2CA4;
    const int MAT_C_START = 0x2CAD;
    const int MAT_D_START = 0x7ACD;
    const int STACK0_START = 0xA200;
    const int STACK1_START = 0xA300;

    const int MEM_END = 0xA300;

    const int MAT_A_SIZE = (MAT_B_START - MAT_A_START);
    const int MAT_B_SIZE = (MAT_C_START - MAT_B_START);
    const int MAT_C_SIZE = (MAT_D_START - MAT_C_START) / 2;
    const int MAT_D_SIZE = (0xA1DD - MAT_D_START);

    std::cout << "MAT A length: " << MAT_A_SIZE << "\n";
    std::cout << "MAT B length: " << MAT_B_SIZE << "\n";
    std::cout << "MAT C length: " << MAT_C_SIZE << "\n";
    std::cout << "MAT D length: " << MAT_D_SIZE << "\n";

    Memory *ram = new Memory(100, MEM_END, {0x200, 0x400, MEM_END});
    MemoryBus *memBus = new MemoryBus(BUS_ARBITRATION_TIME, ram);
    Cpu cpu0 = Cpu(memBus);
    Cpu cpu1 = Cpu(memBus);

    // Initialize matrices in memory
    load_binary("MAT_A_DATA", ram, MAT_A_START);
    load_binary("MAT_B_DATA", ram, MAT_B_START);

    // Set up MAT C with all 1s
    for (int i = 0; i < MAT_C_SIZE; i += 4)
    {
        uint32_t data = 0xFFFFFFFF;
        ram->write(MAT_C_START + (i), data);
    }

    cpu0.addPipeline(new Fetch(&cpu0))
        ->addPipeline(new Decode(&cpu0))
        ->addPipeline(new Execute(&cpu0))
        ->addPipeline(new StorePipeline(&cpu0));

    cpu1.addPipeline(new Fetch(&cpu1))
        ->addPipeline(new Decode(&cpu1))
        ->addPipeline(new Execute(&cpu1))
        ->addPipeline(new StorePipeline(&cpu1));

    cpu0.loadProgram("A5_CPU0.bin");
    cpu1.loadProgram("A5_CPU1.bin", 0x200);

    cpu0.intRegister.write(14, STACK0_START); // Set stack pointer at bottom of stack
    cpu1.intRegister.write(14, STACK1_START); // Set stack pointer at bottom of stack

    for (int i = 0; i < 0x200; i += 4)
    {
        uint32_t data = ram->readUint(i);
        if (data != 0)
        {
            std::cout << i << ": " << RawInstruction(data).keyword() << "\n";
        }
        else
        {
            std::cout << i << ": " << data << "\n";
        }
    }

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, &cpu0));
    masterEventQueue.push(new Event("Tick", 0, &cpu1));

    while (!cpu0.complete || !cpu1.complete)
    {
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    std::cout << "Cpu1 clock cycles: " << cpu0.clocksProcessed << "\n";
    std::cout << "Cpu1 instructions processed: " << cpu0.instructionsProcessed << "\n";
    std::cout << "Cpu1 cpi: " << cpu0.cpi() << "\n";
    std::cout << "Cpu2 clock cycles: " << cpu1.clocksProcessed << "\n";
    std::cout << "Cpu2 instructions processed: " << cpu1.instructionsProcessed << "\n";
    std::cout << "Cpu2 cpi: " << cpu1.cpi() << "\n";
}