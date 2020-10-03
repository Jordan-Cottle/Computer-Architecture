
/*
    Author: Jordan Cottle
    Created: 10/02/2020
*/

#include <string>

#ifndef __MISC__
#define __MISC__

struct printable
{
    virtual std::string __str__() = 0;
};

std::string str(printable *obj)
{
    return obj->__str__();
}

std::string str(printable &obj)
{
    return obj.__str__();
}

#endif
