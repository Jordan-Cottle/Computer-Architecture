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

    std::string line = "";
    int mem_address = 0;

    // Display memory words ordered msb -> lsb
    for (auto item : this->data)
    {
        // Use hex digits, display msb -> lsb
        line = HEX_CHARS[(item & 0x0F)] + line;
        line = HEX_CHARS[(item & 0xF0) >> 4] + line;

        // Group bytes into a word per line
        if (mem_address % 4 == 3)
        {
            s += "\n\t" + str(mem_address - 3) + ": " + line;
            line = "";
        }
        else // Not a word boundary, split bytes
        {
            line = " " + line;
        }

        ++mem_address;
    }

    s += "\n}";

    return s;
}
