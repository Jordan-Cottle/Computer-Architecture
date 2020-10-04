""" Module for test cases that verify the simulation_device.py module. """

from unittest import TestCase

from devices import Device


class TestDevice(TestCase):
    """ Verify base functionality of a Device. """

    def test_device(self):
        """ Place holder test for when Device gets more functionality. """
        device = Device("Test")

        self.assertEqual(device.name, "Test")
        self.assertIn("Test", str(device))