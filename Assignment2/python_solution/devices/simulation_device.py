"""
Module for base definition of a simulation device
"""

from utils import EnumeratedObject, type_of


class Device(EnumeratedObject):
    """ Base class for all devices in the simulation. """

    def __init__(self, name):
        super().__init__()
        self.name = name

    def __str__(self):
        return f"{self.name}"

    def __repr__(self):
        return f'{type_of(self)}(name="{self.name}")'
