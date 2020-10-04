"""
    Module for base Event and the EventQueue definitions
"""

from collections import defaultdict


class EventQueue:
    """ Class to represent a queue of events. """

    def __init__(self):
        self.events = defaultdict(dict)
        # time: {
        #   device: event,
        #   device: event,
        #   ...
        # }

    def queued_for(self, device):
        """ Get events scheduled for a device. """
        return [time[device] for time in self.events.values() if device in time]

    def schedule(self, event):
        """ Push an event into the queue. """
        time = self.events[event.time]

        if event.device in time:
            raise ValueError(
                f"{event} cannot be scheduled at {event.time} due to "
                f"{event.device} already being busy during that time."
            )

        time[event.device] = event

    def pull(self, time):
        """ Pop all events at a given time from the queue. """

        return self.events.pop(time, {}).values()

    def peek(self, time):
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
