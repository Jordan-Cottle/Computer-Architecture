/*
    Author: Jordan Cottle
    Created: 10/03/2020
*/

#ifndef __REGISTER__
#define __REGISTER__

#include <vector>
#include <iostream>

#include "enumerated_object.h"

template <typename T>
struct Register : EnumeratedObject
{
    std::vector<T> addresses;
    int size;
    Register(int size) : EnumeratedObject("Register")
    {
        this->addresses = std::vector<T>(size);
        this->size = size;
    }
    Register(int size, std::string type) : EnumeratedObject(type)
    {
        this->addresses = std::vector<T>(size);
        this->size = size;
    }

    void clear()
    {
        // A Register should never be the single source of truth for any of its contents
        // Do not delete pointers here
        this->addresses.clear();
        this->addresses.resize(this->size);
    }

    void clear(int index)
    {
        // A Register should never be the single source of truth for any of its contents
        // Do not delete pointers here
        this->addresses.erase(this->addresses.begin() + index);
        this->addresses.resize(this->size);
    }

    T read(int index)
    {
        return this->addresses.at(index);
    }

    void write(int index, T item)
    {
        this->addresses[index] = item;
    }

    std::string __str__()
    {
        std::string s = this->type + ": {";

        int mem_address = 0;
        for (auto item : this->addresses)
        {
            s += "\n\t" + std::to_string(mem_address++) + ": ";

            if (item == NULL)
            {
                s += "None";
            }
            else
            {
                s += str(item);
            }
        }

        s += "\n}";

        return s;
    }
};

#endif
