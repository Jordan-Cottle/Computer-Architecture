#include "misc.h"

std::string str(printable *obj)
{
    return obj->__str__();
}

std::string str(printable &obj)
{
    return obj.__str__();
}

std::string str(Printable *obj)
{
    return obj->__str__();
}

std::string str(Printable &obj)
{
    return obj.__str__();
}