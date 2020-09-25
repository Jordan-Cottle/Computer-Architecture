from collections import defaultdict

from utils import type_of


class EnumeratedObject:
    counts = defaultdict(int)

    def __init__(self):
        cls = type(self)
        self.id = EnumeratedObject.counts[cls] + 1
        EnumeratedObject.counts[cls] += 1

    def __str__(self):
        return f"{type(self).__qualname__} ID: {self.id}"

    def __hash__(self):
        return self.id

    def __eq__(self, other):
        if not isinstance(other, type(self)):
            return False

        return self.id == other.id
