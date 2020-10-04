""" Base class definition for objects that should have enumerated instances. """

from collections import defaultdict

from utils import type_of


class EnumeratedObject:
    """
    Base class that will enumerate all instances of a sub-class.

    Each unique sub-class gets a unique enumeration.
    """

    counts = defaultdict(int)

    def __init__(self):
        cls = type(self)
        self.id = EnumeratedObject.counts[cls] + 1
        EnumeratedObject.counts[cls] += 1

    def __str__(self):
        return f"{type_of(self)} ID: {self.id}"

    def __hash__(self):
        return self.id

    def __eq__(self, other):
        if not isinstance(other, type(self)):
            return False

        return self.id == other.id
