""" Test module for event_queue.py. """

from unittest import TestCase

from events import IntEvent, EventQueue


class TestEvent(TestCase):
    """ Verify basic functionality about the Event class. """

    def test_event(self):
        """ Verify a subclass of event can function. """

        event = IntEvent(1, "test", 2)

        new_event = event.process()

        self.assertEqual(new_event.time, 3)
        self.assertEqual(new_event.device, event.device)
        self.assertEqual(new_event.value, event.value)

        self.assertLess(event, new_event)


class TestEventQueue(TestCase):
    """ Verify functionality of EventQueue class. """

    def setUp(self):
        """ Set up a fresh event Queue. """

        self.event_queue = EventQueue()

    def test_event_schedule(self):
        """ Verify that scheduling an event functions properly. """
        event = IntEvent(1, "test", 1)

        self.event_queue.schedule(event)

        self.assertIn(event.time, self.event_queue.events)
        self.assertIn(event.device, self.event_queue.events[event.time])

        conflicting_event = IntEvent(1, "test", 2)
        with self.assertRaises(ValueError):
            self.event_queue.schedule(conflicting_event)

    def test_event_queue_access(self):
        """ Verify that events can be retrieved from the queue. """
        events_per_device = 4
        for i in range(events_per_device):
            event = IntEvent(i, "test", 1)
            alt_event = IntEvent(i, "alt_test", i)

            self.event_queue.schedule(event)
            self.event_queue.schedule(alt_event)

        self.assertEqual(len(self.event_queue), events_per_device * 2)

        self.assertIn(event.device, self.event_queue.peek(i))
        self.assertIn(alt_event.device, self.event_queue.peek(i))
        self.assertEqual(len(self.event_queue.peek(i)), 2)

        events = self.event_queue.pull(i)

        # Ensure data is retrieved
        self.assertIn(event, events)
        self.assertIn(alt_event, events)
        self.assertEqual(len(events), 2)

        # Ensure pull removed events from that time slot
        self.assertEqual(len(self.event_queue.pull(i)), 0)
        # But not other time slots
        self.assertEqual(len(self.event_queue.peek(i - 1)), 2)

        # Re-schedule the event
        self.event_queue.schedule(event)
        queued_for = self.event_queue.queued_for(event.device)

        self.assertEqual(len(queued_for), 4)

        queued_for = self.event_queue.queued_for(alt_event.device)
        self.assertEqual(len(queued_for), 3)
