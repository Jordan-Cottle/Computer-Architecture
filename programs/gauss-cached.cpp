#include "test.h"
#include "memory_router.h"
using namespace Simulation;

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

Memory *ram;
MemoryBus *memBus;
Cpu *cpu0;
Cpu *cpu1;

Cpu *constructCpu(MemoryBus *memBus)
{
    Cache *instructionCache = new Cache(256, 128, DIRECT_MAPPED, memBus);
    instructionCache->type = "Instruction Cache";
    Cache *dataCache = new Cache(512, 128, 4, memBus);
    dataCache->type = "Data Cache";

    MemoryRouter *router = new MemoryRouter(instructionCache, dataCache);
    router->type = "Memory Router";

    Cpu *cpu = new Cpu(router);

    cpu->addPipeline(new Fetch(cpu))
        ->addPipeline(new Decode(cpu))
        ->addPipeline(new Execute(cpu))
        ->addPipeline(new StorePipeline(cpu));

    return cpu;
}

int main()
{

    ram = new Memory(100, MEM_END, {0x200, 0x400, MEM_END});
    memBus = new MemoryBus(BUS_ARBITRATION_TIME, ram);
    cpu0 = constructCpu(memBus);
    cpu1 = constructCpu(memBus);

    std::cout << "MAT A length: " << MAT_A_SIZE << "\n";
    std::cout << "MAT B length: " << MAT_B_SIZE << "\n";
    std::cout << "MAT C length: " << MAT_C_SIZE << "\n";
    std::cout << "MAT D length: " << MAT_D_SIZE << "\n";

    // Initialize matrices in memory
    load_binary("MAT_A_DATA.dat", ram, MAT_A_START);
    load_binary("MAT_B_DATA.dat", ram, MAT_B_START);

    // Set up MAT C with all -1s
    for (int i = MAT_C_START; i < MAT_D_START; i += 1)
    {
        ram->data[i] = 0xFF;
    }

    cpu0->loadProgram("A5_CPU0.bin", 0, ram);
    cpu1->loadProgram("A5_CPU1.bin", 0x200, ram);

    cpu0->intRegister.write(14, STACK0_START); // Set stack pointer at bottom of stack
    cpu1->intRegister.write(14, STACK1_START); // Set stack pointer at bottom of stack

    // Set up initial cpu tick to kick things off
    masterEventQueue.push(new Event("Tick", 0, cpu0));
    masterEventQueue.push(new Event("Tick", 0, cpu1));

    std::cout << "Thinking... (This should take a minute or two)\n";
    while (!cpu0->complete || !cpu1->complete)
    {
        masterEventQueue.tick(simulationClock.cycle);

        simulationClock.tick();
    }
    std::cout << "Program complete!\n";

    std::ofstream outfile;
    outfile.open("cached-convolution.txt");
    for (uint32_t i = MAT_C_START; i < MAT_D_START; i += 2)
    {
        uint16_t data = *(uint16_t *)&ram->data[i];
        outfile << str(data) << "\n";
    }
    outfile.close();

    outfile.open("cached-output.txt");
    for (uint32_t i = MAT_D_START; i < 0xA1DD; i++)
    {
        uint8_t data = ram->data[i];
        outfile << str(data) << "\n";
    }
    outfile.close();

    std::cout << "Cpu1 clock cycles: " << cpu0->clocksProcessed << "\n";
    std::cout << "Cpu1 instructions processed: " << cpu0->instructionsProcessed << "\n";
    std::cout << "Cpu1 cpi: " << cpu0->cpi() << "\n";
    std::cout << "Cpu2 clock cycles: " << cpu1->clocksProcessed << "\n";
    std::cout << "Cpu2 instructions processed: " << cpu1->instructionsProcessed << "\n";
    std::cout << "Cpu2 cpi: " << cpu1->cpi() << "\n";
}