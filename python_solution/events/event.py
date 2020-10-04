"""
Module for base Event class
"""
from abc import ABC, abstractmethod

from utils import EnumeratedObject


class Event(EnumeratedObject, ABC):
    """ Base class for tracking events associated with devices. """

    def __init__(self, trigger_time, device):
        super().__init__()

        self.time = trigger_time
        self.device = device

    def __repr__(self):
        return f"{self.device} Event {self.id} T:{self.time}"

    @abstractmethod
    def process(self):
        """ Standard interface for processing of an event. """
