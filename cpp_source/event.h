#ifndef EVENT_H
#define EVENT_H

class Event
{
public:
    int time;
    int value;
    int id;
    Event(int time, int value);

    bool operator<(const Event &);

    Event *process();
};

#endif