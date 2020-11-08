/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#ifndef __MEMORY__
#define __MEMORY__

#include <vector>
#include <iostream>

#include "device.h"

struct Memory : SimulationDevice
{
    std::vector<uint8_t> data;
    int accessTime;
    std::vector<uint32_t> partitions;

    Memory(uint32_t size, int accessTime);
    Memory(uint32_t size, int accessTime, std::vector<uint32_t> partitions);

    template <typename T>
    T read(uint32_t address)
    {
        return *(T *)&this->data[address];
    }

    template <typename T>
    void write(uint32_t address, T value)
    {
        uint8_t *start = (uint8_t *)&value;

        for (uint32_t i = 0; i < sizeof(value); i++)
        {
            this->data[address + i] = *(start + i);
        }
    }

    std::string __str__();
};

#endif
