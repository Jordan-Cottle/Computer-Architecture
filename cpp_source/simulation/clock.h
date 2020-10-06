/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __CLOCK__
#define __CLOCK__

#include "misc.h"

struct Clock : printable
{

    ulong cycle;
    Clock();

    ulong tick();

    std::string __str__();
};

#endif
