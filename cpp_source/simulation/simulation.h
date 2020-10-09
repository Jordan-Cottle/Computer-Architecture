/*
    Author: Jordan Cottle
    Created: 10/08/2020
*/

#ifndef __SIMULATION__
#define __SIMULATION__

#include "event_queue.h"
#include "clock.h"

namespace Simulation
{
    static EventQueue masterEventQueue;
    static Clock simulationClock;
} // namespace Simulation

#endif
