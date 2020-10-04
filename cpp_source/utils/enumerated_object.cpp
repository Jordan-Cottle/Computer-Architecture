
/*
    Author: Jordan Cottle
    Created: 10/02/2020

    Base struct definition for objects that should have enumerated instances.
*/

#include <unordered_map>
#include <string>
#include <iostream>

#include "misc.h"
#include "enumerated_object.h"

EnumeratedObject::EnumeratedObject(std::string type)
{
    this->type = type;

    this->id = EnumeratedObject::counts[type] + 1;

    EnumeratedObject::counts[type] = this->id;
}

bool EnumeratedObject::operator==(const EnumeratedObject &other)
{
    return this->id == other.id;
}

std::string EnumeratedObject::__str__()
{
    return this->type + " object " + str(this->id);
}

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