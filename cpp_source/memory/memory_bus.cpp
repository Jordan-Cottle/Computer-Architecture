/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/
#include "memory_bus.h"

#include "cache.h"

#include "simulation.h"
using namespace Simulation;

MemoryRequest::MemoryRequest(uint32_t address, SimulationDevice *device, uint32_t completeAt, bool read) : device(device)
{
    this->address = address;
    this->completeAt = completeAt;
    this->read = read;
}

MesiEvent::MesiEvent(MesiSignal signal, uint32_t address, Cache *originator)
{
    this->signal = signal;
    this->address = address;
    this->originator = originator;
}

bool MemoryRequest::operator<(const MemoryRequest &other)
{
    return this->completeAt < other.completeAt;
}

std::string MemoryRequest::__str__()
{
    this->type = "MemoryBus";

MemoryBus::MemoryBus(int accessTime, Memory *memory) : MemoryInterface(accessTime, 0), memory(memory)
{
    this->busyFor = std::vector<uint32_t>(memory->partitions.size());
    this->requests = MinHeap<MemoryRequest *>();
    this->caches = std::vector<Cache *>();
}

void MemoryBus::linkCache(Cache *cache)
{
    this->caches.push_back(cache);
}

void MemoryBus::broadcast(MesiEvent *mesiEvent)
{
    for (auto cache : this->caches)
    {
        if (cache == mesiEvent->originator)
        {
            continue;
        }

        cache->snoop(mesiEvent);
    }
}

Cache *MemoryBus::trackedBy(uint32_t address, Cache *local)
{
    for (auto cache : this->caches)
    {
        if (cache == local)
        {
            continue;
        }

        if (cache->state(address) != INVALID)
        {
            return cache;
        }
    }

    return NULL;
}

uint32_t MemoryBus::port(uint32_t address)
{
    return this->memory->partition(address);
}

bool MemoryBus::request(uint32_t address, SimulationDevice *device, bool read)
{
    uint32_t port = this->port(address);
    this->busyFor[port] += this->accessTime;
    uint32_t completeAt = simulationClock.cycle + this->busyFor[port];

    this->requests.push(new MemoryRequest(address, device, completeAt, read));
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
        OUT << "Processing " << str(request) << "\n";

        bool accepted = this->memory->request(request->address, request->device, request->read);

        if (accepted)
        {
            OUT << "Memory accepted " << request << "\n";
            this->busyFor[this->port(request->address)] -= this->accessTime;
            this->requests.pop();
            delete request;
        }
        else
        {
            OUT << "Memory rejected " << request << "\n";
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

void MemoryBus::write(uint32_t address, void *start, uint32_t bytes)
{
    this->memory->write(address, start, bytes);
}

std::string MemoryBus::__str__()
{
    return "Memory Bus for " + str(this->memory);
}
