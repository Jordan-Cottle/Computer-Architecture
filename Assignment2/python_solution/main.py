from random import choice, randint

from events import EventQueue, Event
from devices import Device

DEVICES = [Device("CPU"), Device("Memory")]


def generate_event_queue():
    """ Set 20 events in a queue. """

    queue = EventQueue()

    for i in range(20):
        device = choice(DEVICES)
        event = Event(randint(0, i), device)
        print(f"Adding {event}")
        queue.schedule(event)

    return queue


def main():
    """ Process 100 events. """

    queue = generate_event_queue()

    time = 0
    count = 0

    while count < 100:
        events = queue.pull(time)
        for event in events:
            new_event = event.process()
            count += 1
            queue.schedule(new_event)

        time += 1


if __name__ == "__main__":

    main()
