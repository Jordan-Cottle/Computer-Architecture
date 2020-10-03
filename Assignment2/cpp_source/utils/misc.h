
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

    virtual ~printable(){};
};

std::string str(printable *obj);
std::string str(printable &obj);

class Printable
{
public:
    virtual std::string __str__() = 0;

    virtual ~Printable(){};
};

std::string str(Printable *obj);
std::string str(Printable &obj);

#endif
