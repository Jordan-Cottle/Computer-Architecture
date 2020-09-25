from random import randint
from collections import defaultdict

from utils import EnumeratedObject


class Event(EnumeratedObject):
    """ Base class for tracking events associated with devices. """

    def __init__(self, trigger_time, device):
        super().__init__()

        self.time = trigger_time
        self.device = device

        print(f"{self} created")

    def __repr__(self):
        return f"{self.device} Event {self.id} T:{self.time}"

    def __lt__(self, value):
        return self.time < value.time

    def process(self):
        """ Assignment 1, process a single event. """

        print(f"Processing {self}")

        return Event(self.time + randint(1, 10), self.device)


class EventQueue:
    """ Class to represent a queue of events. """

    def __init__(self):
        self.events = defaultdict(dict)
        # time: {
        #   device: event,
        #   device: event,
        #   ...
        # }

    def events(self, device):
        """ Get events scheduled for a device. """
        return [time[device] for time in self.events.values()]

    def schedule(self, event):
        """ Push an event into the queue. """
        time = self.events[event.time]

        if event.device in time:
            raise ValueError(
                f"{event} cannot be scheduled at {event.time} due to {event.device} already being busy during that time."
            )

        time[event.device] = event

    def pull(self, time):
        """ Pop all events at a given time from the queue. """

        return self.events.pop(time, {}).values()

    def read(self, time):
        """ Get events in queue at a time without removing them. """

        return self.events[time]

    def __repr__(self):
        return f"EventQueue: {repr(self.events)}"

    def __len__(self):
        """ Get length of the queue. """
        count = 0
        for time in self.events.values():
            count += len(time)

        return count