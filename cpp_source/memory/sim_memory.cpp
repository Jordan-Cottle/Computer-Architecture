/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#include "sim_memory.h"

std::string HEX_CHARS[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

Memory::Memory(int size, int accessTime) : SimulationDevice("Memory")
{
    this->data = std::vector<uint8_t>(size);
    this->accessTime = accessTime;
}

std::string Memory::__str__()
{
    std::string s = "Memory : {";

    int mem_address = 0;
    for (auto item : this->data)
    {
        s += "\n\t" + str(mem_address++) + ": ";

        s += HEX_CHARS[(item & 0xF0) >> 4];
        s += HEX_CHARS[(item & 0x0F)];
    }

    s += "\n}";

    return s;
}
