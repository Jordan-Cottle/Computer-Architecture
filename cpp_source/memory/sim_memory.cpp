/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#include "sim_memory.h"
#include "simulation.h"
using namespace Simulation;

std::string HEX_CHARS[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

Memory::Memory(uint32_t size, int accessTime) : SimulationDevice("Memory")
{
    this->data = std::vector<uint8_t>(size);
    this->accessTime = accessTime;

    this->partitions = {size};
    this->busy = {false};
}

Memory::Memory(uint32_t size, int accessTime, std::vector<uint32_t> partitions) : SimulationDevice("Memory")
{
    this->data = std::vector<uint8_t>(size);
    this->accessTime = accessTime;

    this->partitions = partitions;

    uint32_t length = 0;
    for (auto partition : partitions)
    {
        length += partition;
        this->busy.push_back(false);
    }

    if (length != size)
    {
        throw std::runtime_error("Memory must be completely broken into partitions!");
    }
}

uint32_t Memory::partition(uint32_t address)
{
    uint32_t i = 0;
    uint32_t j = address;
    while (j >= this->partitions[i])
    {
        j -= this->partitions[i++];
    }
    return i;
}

bool Memory::request(uint32_t address, SimulationDevice *device)
{
    uint32_t partition = this->partition(address);

    if (this->busy[partition])
    {
        return false;
    }

    this->busy[partition] = true;

    Event *event = new Event("MemoryReady", simulationClock.cycle + this->accessTime, device);
    masterEventQueue.push(event);

    return true;
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
