import time
from heapq import heappop, heappush
from random import randint, random


class Event:
    count = 0

    def __init__(self, trigger_time, value):
        self.time = trigger_time
        self.value = value
        self.id = Event.count + 1

        Event.count += 1
        print(f"{self} created")

    def __repr__(self):
        return f"Event {self.id} (T:{self.time} V:{self.value})"

    def __lt__(self, value):
        return self.time < value.time

    def process(self):
        """ Assignment 1, process a single event. """

        print(f"Processing {self}")

        return Event(self.time + randint(1, self.value), self.value)


class EventQueue:
    def __init__(self):
        self.events = []

    def push(self, event):
        """ Push an event into the queue. """

        heappush(self.events, event)

    def pop(self):
        """ Pop an event from the queue. """

        return heappop(self.events)

    def top(self):
        """ Get top event in queue without removing it. """

        return self.events[0]

    def __repr__(self):
        return f"EventQueue: {repr(self.events)}"

    def __len__(self):
        """ Get length of the queue. """

        return len(self.events)


def generate_event_queue():
    """ Set 20 events in a queue. """

    queue = EventQueue()

    for i in range(20):
        value = randint(1, 10)
        event = Event(randint(0, i), value)
        print(f"Adding {event}")
        queue.push(event)

    return queue


def main():
    """ Process 100 events. """

    queue = generate_event_queue()

    for _ in range(100):
        event = queue.pop()
        new_event = event.process()
        queue.push(new_event)


if __name__ == "__main__":

    main()
