/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#include "sim_memory.h"
#include "simulation.h"
using namespace Simulation;

std::string HEX_CHARS[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

MemoryInterface::MemoryInterface(uint32_t accessTime) : SimulationDevice("Memory")
{
    this->accessTime = accessTime;
}

Memory::Memory(int accessTime, uint32_t size) : MemoryInterface(accessTime)
{
    this->data = std::vector<uint8_t>(size);
    this->accessTime = accessTime;

    this->partitions = {size};
    this->busy = {false};
}

Memory::Memory(int accessTime, uint32_t size, std::vector<uint32_t> partitions) : MemoryInterface(accessTime)
{
    this->data = std::vector<uint8_t>(size);
    this->accessTime = accessTime;

    this->partitions = partitions;

    for (auto partition : partitions)
    {
        if (partition > size)
        {
            throw std::runtime_error("Partition " + str(partition) + " for memory cannot be bigger than its size " + str(size));
        }
        this->busy.push_back(false);
    }
}

uint32_t Memory::partition(uint32_t address)
{
    if (address > this->data.size())
    {
        throw std::runtime_error("Access is outside of the memory's range");
    }

    uint32_t i = 0;
    while (address >= this->partitions[i])
    {
        i++;
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

uint32_t Memory::readUint(uint32_t address)
{
    this->busy[this->partition(address)] = false;
    return *(uint32_t *)&this->data[address];
}

int Memory::readInt(uint32_t address)
{
    this->busy[this->partition(address)] = false;
    return *(int *)&this->data[address];
}

float Memory::readFloat(uint32_t address)
{
    this->busy[this->partition(address)] = false;
    return *(float *)&this->data[address];
}

void Memory::write(uint32_t address, uint32_t value)
{
    this->busy[this->partition(address)] = false;

    uint8_t *start = (uint8_t *)&value;
    for (uint32_t i = 0; i < sizeof(value); i++)
    {
        this->data[address + i] = *(start + i);
    }
}

void Memory::write(uint32_t address, int value)
{
    this->busy[this->partition(address)] = false;

    uint8_t *start = (uint8_t *)&value;
    for (uint32_t i = 0; i < sizeof(value); i++)
    {
        this->data[address + i] = *(start + i);
    }
}

void Memory::write(uint32_t address, float value)
{
    this->busy[this->partition(address)] = false;

    uint8_t *start = (uint8_t *)&value;
    for (uint32_t i = 0; i < sizeof(value); i++)
    {
        this->data[address + i] = *(start + i);
    }
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
