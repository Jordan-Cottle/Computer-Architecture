/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#include "simulation.h"

namespace Simulation
{
    EventQueue masterEventQueue;
    Clock simulationClock;
    Cpu cpu;
    Cpu cpu2;
    Memory ram = Memory(MEMORY_SIZE, MEMORY_DELAY, {0x100, 0x200 - 0x100, 0x1400 - 0x200});
    MemoryBus memBus = MemoryBus(BUS_ARBITRATION_TIME, &ram);

} // namespace Simulation
