/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __SIMULATION__
#define __SIMULATION__

#include "event_queue.h"
#include "clock.h"
#include "cpu.h"

constexpr int REGISTER_COUNT = 32;
constexpr int MEMORY_SIZE = 0x1400;
constexpr int MEMORY_DELAY = 20;
constexpr int SIM_CYCLES_PER_CPU = 10;
constexpr int MEMORY_ADDRESSES_PER_INSTRUCTION = 4;
constexpr uint32_t BUS_ARBITRATION_TIME = 5;

namespace Simulation
{
    extern EventQueue masterEventQueue;
    extern Clock simulationClock;
    extern Cpu cpu;
} // namespace Simulation

#endif
