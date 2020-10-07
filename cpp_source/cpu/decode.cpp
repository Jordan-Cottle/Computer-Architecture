/*
    Author: Jordan Cottle
    Created: 10/05/2020
*/

#include "decode.h"
Decode::Decode() : Pipeline("Decode")
{
}

void Decode::tick(ulong time, EventQueue *eventQueue)
{
    Instruction *staged = this->staged();

    if (staged == NULL)
    {
        std::cout << "No instruction to decode\n";
    }
    else
    {
        std::cout << "Decoding: " << this->staged() << "\n";
    }

    Pipeline::tick(time, eventQueue);
}
