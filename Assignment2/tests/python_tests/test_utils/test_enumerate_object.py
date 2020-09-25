from unittest import TestCase

from utils import EnumeratedObject


class A(EnumeratedObject):
    pass


class B(EnumeratedObject):
    pass


class C(B):
    pass


class TestEnumerateObject(TestCase):
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

        a = A()
        b = B()
        c = C()

        dic = {
            a: 1,
            b: 2,
            c: 3,
            A(): 4,
            B(): 5,
            C(): 6,
        }

        self.assertIn(a, dic)
        self.assertIn(b, dic)
        self.assertIn(c, dic)

        self.assertEqual(dic[a], 1)
        self.assertEqual(dic[b], 2)
        self.assertEqual(dic[c], 3)

        self.assertEqual(len(dic), 6)