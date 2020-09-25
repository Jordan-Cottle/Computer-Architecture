from unittest import TestCase

from utils import type_of
from utils import EnumeratedObject


def foo():
    pass


class A(EnumeratedObject):
    pass


class B(EnumeratedObject):
    pass


class C(B):
    pass


class TestUtils(TestCase):
    """ Test class for utils """

    def test_type_of(self):
        self.assertEqual(type_of(foo), "function")
        self.assertEqual(type_of(EnumeratedObject()), "EnumeratedObject")
        self.assertEqual(type_of(EnumeratedObject), "type")

    def test_enumerated_object_ids(self):

        last_id = 0
        for _ in range(1, 10):
            c = C()
            b = B()
            a = A()

            self.assertEqual(a.id, b.id, msg="Ids should be set independently")
            self.assertEqual(b.id, c.id, msg="Ids should be set independently")

            self.assertGreater(
                a.id, last_id, msg="Ids should increment with each new object"
            )
            last_id = a.id

    def test_enumerated_object_hashable(self):
        """ Verify that multiple types can be stored into the same dictionary. """

        dic = {}

        a = A()
        b = B()
        c = C()

        dic[a] = 1
        dic[b] = 2
        dic[c] = 3

        self.assertIn(a, dic)
        self.assertIn(b, dic)
        self.assertIn(c, dic)

        self.assertEqual(dic[a], 1)
        self.assertEqual(dic[b], 2)
        self.assertEqual(dic[c], 3)

        self.assertEqual(len(dic), 3)


if __name__ == "__main__":
    from unittest.main import main

    main()