/*
    Author: Jordan Cottle
    Created: 11/08/2020
*/
#include "memory_bus.h"

#include "cache.h"

#include "simulation.h"
using namespace Simulation;

MesiEvent::MesiEvent(MesiSignal signal, uint32_t address, Cache *originator)
{
    this->signal = signal;
    this->address = address;
    this->originator = originator;
}

MemoryBus::MemoryBus(int accessTime, MemoryController *memory) : MemoryInterface(accessTime, 0), memory(memory)
{
    this->type = "MemoryBus";
    this->requests = std::vector<std::deque<MemoryRequest *> *>();

    for (uint32_t i = 0; i < memory->memoryBanks.size(); i++)
    {
        this->requests.push_back(new std::deque<MemoryRequest *>());
    }
    this->caches = std::vector<Cache *>();

    // Set up first process trigger
    Event *event = new Event("ProcessRequests", simulationClock.cycle + this->accessTime, this, HIGH);
    masterEventQueue.push(event);
}

void MemoryBus::linkCache(Cache *cache)
{
    this->caches.push_back(cache);
}

void MemoryBus::broadcast(MesiEvent *mesiEvent)
{
    WriteBack *writeBack = NULL;
    for (auto cache : this->caches)
    {
        if (cache == mesiEvent->originator)
        {
            continue;
        }

        try
        {
            cache->snoop(mesiEvent);
        }
        catch (WriteBack &writeBackSignal)
        {
            if (writeBack != NULL)
            {
                throw std::logic_error("Only one write back should be needed per broadcast!!");
            }

            Event *event = new Event("WriteBack", simulationClock.cycle + this->memory->accessTime, cache, HIGH);
            masterEventQueue.push(event);
            writeBack = &writeBackSignal;
        }
    }

    if (writeBack != NULL)
    {
        // Reschedule write back ahead of current memory request
        if (mesiEvent->address != writeBack->address)
        {
            throw std::logic_error("The write back should be associated with the same address as the mesi event!");
        }

        // Get first request in queue, it should be the one that triggered this mesi event
        uint32_t port = this->port(writeBack->address);
        auto requestQueue = this->requests.at(port);
        auto request = requestQueue->front();

        // Validate assumptions about the context here
        if (request->inProgress)
        {
            throw std::logic_error("A write back should never be triggered for a request that is already progress!");
        }

        if (request->address != writeBack->address)
        {
            throw std::logic_error("The write back address should match request that is in progress!");
        }

        if (request->device == writeBack->cache)
        {
            throw std::logic_error("The cache making a write back should NOT be the cache making the original request!");
        }

        Cache *cache = writeBack->cache;
        cache->writeBackRequest = new MemoryRequest(writeBack->address, cache, false);
        cache->writeBackRequest->inProgress = true;

        // Write back request should go first
        requestQueue->push_front(cache->writeBackRequest);
    }
    delete mesiEvent;
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

bool MemoryBus::request(MemoryRequest *request)
{
    this->requests.at(this->port(request->address))->push_back(request);

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
            assert(request != NULL);
            if (request->inProgress)
            {
                DEBUG << "Memory bus waiting for a request to complete\n";
                DEBUG << "Request: " << str(request) << "\n";
                continue; // Request already in progress
            }

            DEBUG << "Processing " << str(request) << "\n";

            bool accepted = this->memory->request(request);

            if (accepted)
            {
                DEBUG << this << " redirected " << request << " accepted\n";
                assert(request->inProgress);
            }

            DEBUG << "Port " << str(port) << ": " << str(requestQueue->size()) << " event(s) left\n";
        }

        Event *nextProcess = new Event("ProcessRequests", simulationClock.cycle + this->accessTime, this, HIGH);
        masterEventQueue.push(nextProcess);
    }

    SimulationDevice::process(event);
}

void MemoryBus::cancelRequest(MemoryRequest *request)
{
    DEBUG << "Memory bus is canceling " << request << "\n";
    if (!request->enqueued)
    {
        return; // We haven't seen this request yet
    }

    assert(request->inProgress);
    this->clearRequest(request);
    this->memory->cancelRequest(request);
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

void MemoryBus::clearRequest(MemoryRequest *request)
{
    uint32_t port = this->port(request->address);
    auto requestQueue = this->requests.at(port);

    MemoryRequest *found = NULL;
    std::deque<MemoryRequest *> lookedAt = std::deque<MemoryRequest *>();
    for (uint32_t i = 0; i < requestQueue->size(); ++i)
    {
        auto enqueuedRequest = requestQueue->front();
        requestQueue->pop_front();
        if (request == enqueuedRequest)
        {
            found = request;
            break;
        }

        // Hold onto this for later, it wasn't the right one
        lookedAt.push_back(enqueuedRequest);
    }

    // Did we get it?
    assert(found != NULL);

    // Put them back gently
    while (!lookedAt.empty())
    {
        auto replace = lookedAt.back();
        lookedAt.pop_back();
        requestQueue->push_front(replace);
    }

    if (request->inProgress)
    {
        request->enqueued = false;
        if (request->canceled)
        {
            delete request; // This is the final reference to it
        }
    }
}

void MemoryBus::clearRequest(uint32_t address)
{
    uint32_t port = this->port(address);
    auto requestQueue = this->requests.at(port);
    if (requestQueue->empty())
    {
        return;
    }

    auto request = requestQueue->front();

    if (request->inProgress)
    {
        requestQueue->pop_front();
        request->enqueued = false;
        if (request->canceled)
        {
            delete request; // This is the final reference to it
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
