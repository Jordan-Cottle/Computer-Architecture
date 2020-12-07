/*
    Author: Jordan Cottle
    Created: 10/30/2020
*/

#include "sim_memory.h"
#include "simulation.h"
using namespace Simulation;

MemoryRequest::MemoryRequest(uint32_t address, SimulationDevice *device, bool read) : device(device)
{
    this->address = address;
    this->read = read;
    this->inProgress = false;
    this->currentHandler = NULL;
    this->enqueued = false;
    this->canceled = false;
}

std::string MemoryRequest::__str__()
{
    return std::string(this->read ? "read" : "write") + " request for address " + str(this->address) + " by " + this->device->type + " currently handled by " + (this->currentHandler == NULL ? "NULL" : this->currentHandler->type);
}

std::string HEX_CHARS[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

MemoryInterface::MemoryInterface(uint32_t accessTime, uint32_t size) : SimulationDevice("Memory")
{
    this->accessTime = accessTime;
    this->size = size;
}

bool MemoryInterface::withinBounds(uint32_t address)
{
    return address < this->size;
}

void MemoryInterface::checkBounds(uint32_t address)
{
    if (!this->withinBounds(address))
    {
        throw std::runtime_error("Access is outside of the memory's range");
    }
}

MemoryBank::MemoryBank(uint32_t accessTime, uint32_t size, uint32_t logicalOffset) : MemoryInterface(accessTime, size)
{
    this->type = "Memory Bank";
    this->data = std::vector<uint8_t>(size);
    this->activeRequest = NULL;
    this->logicalOffset = logicalOffset;
}

bool MemoryBank::request(MemoryRequest *request)
{
    DEBUG << this->type << " received " << request << "\n";
    assert(request->currentHandler != this);
    if (this->busy())
    {
        DEBUG << this->type << " is busy handling " << this->activeRequest << " and cannot handle " << request << "\n";
        return false;
    }

    assert(this->activeRequest == NULL);

    DEBUG << this->type << " started processing " << request << "\n";
    this->activeRequest = request;
    this->activeRequest->currentHandler = this;
    this->activeRequest->inProgress = true;

    Event *event = new Event("MemoryReady", simulationClock.cycle + this->accessTime, this);
    masterEventQueue.push(event);
    return true;
}

void MemoryBank::cancelRequest(MemoryRequest *request)
{
    INFO << "Memory cell canceling " << request << "\n";
    assert(this->activeRequest == request);
    this->activeRequest = NULL;
}

void MemoryBank::process(Event *event)
{
    if (event->type == "MemoryReady")
    {
        event->handled = true;
        if (this->activeRequest == NULL)
        {
            DEBUG << "Canceled request ready in memory bank\n";
        }
        else
        {
            Event *newEvent;
            if (this->activeRequest->read)
            {
                newEvent = new Event("MemoryReadReady", event->time, this->activeRequest->device, HIGH);
            }
            else
            {
                newEvent = new Event("MemoryWriteReady", simulationClock.cycle, this->activeRequest->device, HIGH);
            }

            masterEventQueue.push(newEvent);
        }
    }

    MemoryInterface::process(event);
}

uint32_t MemoryBank::readUint(uint32_t physicalAddress)
{
    this->activeRequest = NULL;
    uint32_t address = this->localAddress(physicalAddress);
    return *(uint32_t *)&this->data.at(address);
}

int MemoryBank::readInt(uint32_t physicalAddress)
{
    this->activeRequest = NULL;
    uint32_t address = this->localAddress(physicalAddress);
    return *(int *)&this->data.at(address);
}

float MemoryBank::readFloat(uint32_t physicalAddress)
{
    this->activeRequest = NULL;
    uint32_t address = this->localAddress(physicalAddress);
    return *(float *)&this->data.at(address);
}

void MemoryBank::write(uint32_t physicalAddress, void *start, uint32_t bytes)
{
    this->activeRequest = NULL;
    uint32_t address = this->localAddress(physicalAddress);

    uint8_t *begin = (uint8_t *)start;
    for (uint32_t i = 0; i < bytes; i++)
    {
        uint8_t byte = *(begin + i);
        DEBUG << "Writing " << str(byte) << " into address " << str(address + i) << "\n";
        this->data.at(address + i) = byte;
    }
}

bool MemoryBank::busy()
{
    return this->activeRequest != NULL;
}

uint32_t MemoryBank::localAddress(uint32_t physicalAddress)
{
    assert(physicalAddress >= this->logicalOffset);
    return physicalAddress - this->logicalOffset;
}

std::string MemoryBank::__str__()
{
    std::string s = "MemoryBank : {";

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

MemoryController::MemoryController(uint32_t accessTime, uint32_t size) : MemoryInterface(accessTime, size)
{
    this->type = "Memory Controller";
    this->memoryBanks = {new MemoryBank(accessTime, size, 0)};
}

MemoryController::MemoryController(uint32_t accessTime, uint32_t size, std::vector<uint32_t> partitions) : MemoryInterface(accessTime, size)
{
    this->memoryBanks = std::vector<MemoryBank *>();

    uint32_t offset = 0;
    for (auto partition : partitions)
    {
        if (partition > size)
        {
            throw std::runtime_error("Partition " + str(partition) + " for memory cannot be bigger than its size " + str(size));
        }

        this->memoryBanks.push_back(new MemoryBank(accessTime, partition - offset, offset));
        offset = partition;
    }
}

uint32_t MemoryController::partition(uint32_t address)
{
    this->checkBounds(address);
    uint32_t i = 0;
    for (auto memoryBank : this->memoryBanks)
        if (memoryBank->logicalOffset <= address)
        {
            i += 1;
        }
    return i - 1;
}

MemoryBank *MemoryController::getBank(uint32_t address)
{
    return this->memoryBanks.at(this->partition(address));
}

bool MemoryController::request(MemoryRequest *request)
{
    uint32_t address = request->address;
    this->checkBounds(address);
    return this->getBank(address)->request(request);
}

void MemoryController::cancelRequest(MemoryRequest *request)
{
    this->getBank(request->address)->cancelRequest(request);
}

uint32_t MemoryController::readUint(uint32_t address)
{
    return this->getBank(address)->readUint(address);
}

int MemoryController::readInt(uint32_t address)
{
    return this->getBank(address)->readInt(address);
}

float MemoryController::readFloat(uint32_t address)
{
    return this->getBank(address)->readFloat(address);
}

void MemoryController::write(uint32_t address, void *start, uint32_t bytes)
{
    this->getBank(address)->write(address, start, bytes);
}
