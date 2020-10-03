#include "misc.h"

std::string str(printable *obj)
{
    return obj->__str__();
}

std::string str(printable &obj)
{
    return obj.__str__();
}

std::ostream &operator<<(std::ostream &output, printable &p)
{
    return output << str(p);
}