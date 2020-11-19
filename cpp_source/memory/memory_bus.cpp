/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/
#include "memory_bus.h"

#include "simulation.h"
using namespace Simulation;

MemoryRequest::MemoryRequest(uint32_t address, SimulationDevice *device, uint32_t completeAt) : device(device)
{
    this->address = address;
    this->completeAt = completeAt;
}

bool MemoryRequest::operator<(const MemoryRequest &other)
{
    return this->completeAt < other.completeAt;
}

std::string MemoryRequest::__str__()
{
    return "Memory request for address " + str(this->address) + " by " + this->device->type + " resolved at time " + str(this->completeAt);
}

MemoryBus::MemoryBus(int accessTime, Memory *memory) : MemoryInterface(accessTime, 0), memory(memory)
{
    this->busyFor = std::vector<uint32_t>(memory->partitions.size());
    this->requests = MinHeap<MemoryRequest *>();
}

uint32_t MemoryBus::port(uint32_t address)
{
    return this->memory->partition(address);
}

bool MemoryBus::request(uint32_t address, SimulationDevice *device)
{
    uint32_t port = this->port(address);
    this->busyFor[port] += this->accessTime;
    uint32_t completeAt = simulationClock.cycle + this->busyFor[port];
    this->requests.push(new MemoryRequest(address, device, completeAt));
    Event *event = new Event("ProcessRequest", completeAt, this);
    masterEventQueue.push(event);

    return true;
}

void MemoryBus::process(Event *event)
{
    if (event->type == "ProcessRequest")
    {
        event->handled = true;
        MemoryRequest *request = this->requests.top();
        // std::cout << "Processing " << str(request) << "\n";

        bool accepted = this->memory->request(request->address, request->device);
        if (accepted)
        {
            // std::cout << "Memory accepted " << request << "\n";
            this->busyFor[this->port(request->address)] -= this->accessTime;
            this->requests.pop();
            delete request;
        }
        else
        {
            // std::cout << "Memory rejected " << request << "\n";
            // Update request expected to complete time
            this->requests.pop();
            request->completeAt += this->accessTime;
            this->requests.push(request);

            // Reschedule event
            Event *event = new Event("ProcessRequest", simulationClock.cycle + this->busyFor[this->memory->partition(request->address)], this, MEDIUM - 1);
            masterEventQueue.push(event);
        }
    }

    SimulationDevice::process(event);
}

uint32_t MemoryBus::readUint(uint32_t address)
{
    return this->memory->readUint(address);
}

int MemoryBus::readInt(uint32_t address)
{
    return this->memory->readInt(address);
}

float MemoryBus::readFloat(uint32_t address)
{
    return this->memory->readFloat(address);
}

void MemoryBus::write(uint32_t address, uint32_t value)
{
    this->memory->write(address, value);
}

void MemoryBus::write(uint32_t address, int value)
{
    this->memory->write(address, value);
}

void MemoryBus::write(uint32_t address, float value)
{
    this->memory->write(address, value);
}

std::string MemoryBus::__str__()
{
    return "Memory Bus for " + str(this->memory);
}
