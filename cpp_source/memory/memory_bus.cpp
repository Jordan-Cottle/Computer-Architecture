/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/
#include "memory_bus.h"

#include "cache.h"

#include "simulation.h"
using namespace Simulation;

MemoryRequest::MemoryRequest(uint32_t address, SimulationDevice *device, bool read) : device(device)
{
    this->address = address;
    this->read = read;
    this->requested = false;
}

MesiEvent::MesiEvent(MesiSignal signal, uint32_t address, Cache *originator)
{
    this->signal = signal;
    this->address = address;
    this->originator = originator;
}

std::string MemoryRequest::__str__()
{
    return "Memory request for address " + str(this->address) + " by " + this->device->type;
}

MemoryBus::MemoryBus(int accessTime, Memory *memory) : MemoryInterface(accessTime, 0), memory(memory)
{
    this->type = "MemoryBus";
    this->requests = std::vector<std::deque<MemoryRequest *> *>();

    for (uint32_t i = 0; i < memory->partitions.size(); i++)
    {
        this->requests.push_back(new std::deque<MemoryRequest *>());
    }
    this->caches = std::vector<Cache *>();

    // Set up first process trigger
    Event *event = new Event("ProcessRequests", simulationClock.cycle + this->accessTime, this);
    masterEventQueue.push(event);
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
    this->requests.at(this->port(address))->push_back(new MemoryRequest(address, device, read));

    return true;
}

void MemoryBus::process(Event *event)
{
    if (event->type == "ProcessRequests")
    {
        event->handled = true;
        int port = -1;
        for (auto requestQueue : this->requests)
        {
            port++;
            if (requestQueue->empty())
            {
                continue;
            }

            MemoryRequest *request = requestQueue->front();
            if (request->requested)
            {
                continue; // Request already in progress
            }

            OUT << "Processing " << str(request) << "\n";

            bool accepted = this->memory->request(request->address, request->device, request->read);

            if (accepted)
            {
                OUT << request << " accepted\n";
                request->requested = true;
            }

            OUT << "Port " << str(port) << ": " << str(requestQueue->size()) << " event(s) left\n";
        }

        Event *nextProcess = new Event("ProcessRequests", simulationClock.cycle + this->accessTime, this);
        masterEventQueue.push(nextProcess);
    }

    SimulationDevice::process(event);
}

void MemoryBus::clearRequest(uint32_t address)
{
    uint32_t port = this->port(address);
    auto requestQueue = this->requests.at(port);
    auto request = requestQueue->front();

    for (auto queue : this->requests)
    {
        if (queue->empty())
        {
            continue;
        }

        auto r = queue->front();

        if (r == NULL)
        {
            throw std::logic_error("Requests in queue should not be null before clearing!");
        }
    }

    if (requestQueue->empty() || !request->requested)
    {
        return; // Multiple read/write calls from single request
    }
    delete request;
    requestQueue->pop_front();

    for (auto queue : this->requests)
    {
        if (queue->empty())
        {
            continue;
        }

        auto r = queue->front();

        if (r == NULL)
        {
            throw std::logic_error("Requests in queue should not be null after clearing!");
        }
    }
}

uint32_t MemoryBus::readUint(uint32_t address)
{
    this->clearRequest(address);
    return this->memory->readUint(address);
}

int MemoryBus::readInt(uint32_t address)
{
    this->clearRequest(address);
    return this->memory->readInt(address);
}

float MemoryBus::readFloat(uint32_t address)
{
    this->clearRequest(address);
    return this->memory->readFloat(address);
}

void MemoryBus::write(uint32_t address, void *start, uint32_t bytes)
{
    this->clearRequest(address);
    this->memory->write(address, start, bytes);
}

std::string MemoryBus::__str__()
{
    return "Memory Bus for " + str(this->memory);
}
