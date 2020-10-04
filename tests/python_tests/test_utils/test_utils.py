from unittest import TestCase

from utils import type_of


def foo():
    pass


class Bar:
    def foo(self):
        pass


class TestUtils(TestCase):
    """ Test class for utils """

    def test_type_of(self):
        self.assertEqual(type_of(foo), "function")
        self.assertEqual(type_of(Bar()), "Bar")
        self.assertEqual(type_of(Bar), "type")
        self.assertEqual(type_of(Bar().foo), "method")
