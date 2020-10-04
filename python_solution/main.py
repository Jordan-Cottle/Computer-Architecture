"""
Main entry module for the simulation
"""


from random import choice, randint

from events import EventQueue, IntEvent
from devices import Device

DEVICES = [Device("CPU"), Device("Memory")]


def schedule_event(queue, event):
    """Schedule an event.

    Move to a later time if there is a conflict.
    """

    try:
        queue.schedule(event)
    except ValueError:  # Reschedule event for later
        event.time += 1
        schedule_event(queue, event)
    else:
        print(f"{event!r} successfully scheduled for time {event.time}")


def populate_event_queue(queue, count=20):
    """ Set 20 events in a queue. """

    for i in range(count):
        device = choice(DEVICES)
        event = IntEvent(randint(0, i), device, 1)
        schedule_event(queue, event)
    return queue


MEQ = EventQueue()


def main():
    """ Process 100 events. """

    populate_event_queue(MEQ)

    time = 0
    count = 0

    while count < 100:
        print(f"Time: {time}")
        events = MEQ.pull(time)
        for event in events:
            new_event = event.process()
            count += 1
            schedule_event(MEQ, new_event)

        time += 1


if __name__ == "__main__":

    main()
