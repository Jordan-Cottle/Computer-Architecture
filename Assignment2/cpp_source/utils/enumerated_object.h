
/*
    Author: Jordan Cottle
    Created: 10/02/2020

    Base struct definition for objects that should have enumerated instances.
*/

#ifndef __ENUMERATED_OBJECT__
#define __ENUMERATED_OBJECT__

#include <unordered_map>
#include <string>

#include "misc.h"

/*
Base struct that will enumerate all instances of a sub-struct.

Each unique sub-struct gets a unique enumeration.
*/
struct EnumeratedObject : printable
{
    int id;
    std::string type;

    EnumeratedObject(std::string type);

    bool operator==(const EnumeratedObject &other);

    virtual std::string __str__();

private:
    static std::unordered_map<std::string, int> counts;
};
#endif
