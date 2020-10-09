/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __SIMULATION__
#define __SIMULATION__

#include "event_queue.h"
#include "clock.h"
#include "cpu.h"

namespace Simulation
{
    extern EventQueue masterEventQueue;
    extern Clock simulationClock;
    extern Cpu cpu;
} // namespace Simulation

#endif
