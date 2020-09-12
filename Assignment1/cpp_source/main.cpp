#include "event_queue.h"
#include "event.h"

#include <iostream>
#include <stdlib.h>

int main()
{
    EventQueue masterEventQueue;

    for (int i = 1; i <= 20; i++)
    {
        Event *event = new Event(rand() % i, rand() % 10 + 1);

        masterEventQueue.push(event);
    }

    std::cout << "Starting simulation...\n";

    for (int i = 0; i < 100; i++)
    {
        Event *event = masterEventQueue.pop();
        Event *new_event = event->process();

        masterEventQueue.push(new_event);
    }
}