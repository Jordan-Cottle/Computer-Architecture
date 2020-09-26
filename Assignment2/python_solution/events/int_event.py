from events import Event


class IntEvent(Event):
    """Basic event from Assignment 1. """

    def __init__(self, time, device, value):
        self.value = value

        super().__init__(time, device)

    def __lt__(self, value):
        return self.time < value.time

    def process(self):
        """ Assignment 1, process a single event. """

        print(f"Processing {self}")

        return IntEvent(self.time + self.value, self.device, self.value)