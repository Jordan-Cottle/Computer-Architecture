""" Verify the behavior of the main module. """

import re
from io import StringIO
from unittest import TestCase
from unittest.mock import patch

from devices import Device
from events import EventQueue, IntEvent
from main import MEQ, populate_event_queue, main, schedule_event

TEST_DEVICE = Device("test")

PROCESSED = re.compile(r"Processing (.+) Event (\d+) T:(\d+)")
SCHEDULED = re.compile(r"(\w+) Event (\d+) T:(\d+) successfully scheduled")


class TestMain(TestCase):
    """ Verify functionality of the main module. """

    def test_int_event_comparison(self):
        """ Verify that the IntEvent class works. """

        first_event = IntEvent(1, TEST_DEVICE, 1)
        second_event = IntEvent(2, TEST_DEVICE, 1)

        self.assertLess(first_event, second_event)

    def test_int_event_process(self):
        """ Verify that the IntEvent class works. """

        event = IntEvent(1, TEST_DEVICE, 1)
        new_event = event.process()

        self.assertEqual(new_event.time, 2)

    def test_schedule_event(self):
        """ Verify events can be scheduled. """

        event = IntEvent(1, TEST_DEVICE, 1)
        queue = EventQueue()

        schedule_event(queue, event)

        self.assertEqual(queue.peek(event.time)[event.device], event)

        second_event = IntEvent(1, TEST_DEVICE, 1)
        schedule_event(queue, second_event)

        self.assertEqual(second_event.time, 2)
        self.assertEqual(
            queue.peek(second_event.time)[second_event.device], second_event
        )

        self.assertEqual(len(queue.queued_for(TEST_DEVICE)), 2)

    def test_populate_event_queue(self):
        """ Verify that the event queue is properly populated. """

        queue = EventQueue()
        populate_event_queue(queue)

        self.assertEqual(len(queue), 20)

    def test_main(self):
        """ Verify that the main sim works properly. """
        print("Starting test for main")
        with patch("sys.stdout", new=StringIO()) as stdout:
            main()

        lines = stdout.getvalue().split("\n")

        scheduled = {}
        processed = {}

        current_time = -1
        for line in lines:
            match = re.search(PROCESSED, line)

            if match:
                event_id = match.group(2)
                time = match.group(3)

                # Verify processing happens in chronological order
                self.assertLessEqual(current_time, int(time))
                current_time = int(time)

                # Verify events are not processed multiple times
                self.assertNotIn(event_id, processed)
                processed[event_id] = time
                continue

            match = re.search(SCHEDULED, line)

            if match:
                event_id = match.group(2)
                time = match.group(3)

                # Verify events are not schedules multiple times
                self.assertNotIn(event_id, scheduled)
                scheduled.setdefault(time, []).append(event_id)
                continue

        incomplete_count = 0
        for time in sorted(scheduled, key=lambda t: int(t)):
            events = scheduled[time]

            for event in events:
                time_completed = processed.pop(event, None)

                if time_completed is None:
                    incomplete_count += 1
                    continue

                self.assertEqual(time_completed, time)

        # Verify all processed events were found in the scheduled list
        self.assertEqual(len(processed), 0)

        # Verify the unprocessed events are all in the queue
        self.assertEqual(len(MEQ), incomplete_count)
