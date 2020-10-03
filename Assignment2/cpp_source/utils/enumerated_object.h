
/*
    Author: Jordan Cottle
    Created: 10/02/2020

    Base struct definition for objects that should have enumerated instances.
*/

#ifndef __ENUMERATED_OBJECT__
#define __ENUMERATED_OBJECT__

#include <unordered_map>
#include <string>
#include <iostream>

#include "misc.h"

/*
Base struct that will enumerate all instances of a sub-struct.

Each unique sub-struct gets a unique enumeration.
*/
struct EnumeratedObject : printable
{
    int id;
    std::string type;

    EnumeratedObject(std::string type)
    {
        this->type = type;

        this->id = EnumeratedObject::counts[type] + 1;

        EnumeratedObject::counts[type] = this->id;
    }

    bool operator==(const EnumeratedObject &other) const
    {
        return this->id == other.id;
    }

    std::string __str__()
    {
        return this->type + " object " + std::to_string(this->id);
    }

private:
    static std::unordered_map<std::string, int> counts;
};

std::unordered_map<std::string, int> EnumeratedObject::counts = std::unordered_map<std::string, int>();

namespace std
{
    template <>
    struct hash<EnumeratedObject>
    {
        std::size_t operator()(const EnumeratedObject &object) const
        {
            return (std::size_t)object.id;
        }
    };

} // namespace std

#endif
