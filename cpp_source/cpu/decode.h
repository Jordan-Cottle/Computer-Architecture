/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#ifndef __DECODE__
#define __DECODE__

#include "pipeline.h"

struct Decode : Pipeline
{

    Decode();

    void tick(ulong time, EventQueue *eventQueue);
};

#endif
