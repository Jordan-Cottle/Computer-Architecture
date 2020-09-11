#include "event_queue.h"
#include "event.h"

#include <iostream>
#include <ctime>
#include <stdlib.h>

int main()
{
    EventQueue masterEventQueue;

    for (int i = 0; i < 20; i++)
    {
        time_t now = time(0);
        tm *time = localtime(&now);
        Event *event = new Event(i, rand() % 10 + 1);

        masterEventQueue.push(event);
    }

    for (int i = 0; i < 100; i++)
    {
        Event *event = masterEventQueue.pop();
        Event *new_event = event->process();

        masterEventQueue.push(new_event);
    }
}