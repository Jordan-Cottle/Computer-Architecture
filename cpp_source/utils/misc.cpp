#include "misc.h"

std::string str(printable *obj)
{
    return obj->__str__();
}

std::string str(printable &obj)
{
    return obj.__str__();
}

std::string str(int num)
{
    return std::to_string(num);
}

std::string str(uint32_t num)
{
    return std::to_string(num);
}

std::string str(ulong num)
{
    return std::to_string(num);
}

std::string str(float num)
{
    return std::to_string(num);
}

std::ostream &operator<<(std::ostream &output, printable &p)
{
    return output << str(p);
}

std::ostream &operator<<(std::ostream &output, printable *p)
{
    return output << str(p);
}

// Modifed from https://thispointer.com/find-and-replace-all-occurrences-of-a-sub-string-in-c/
std::string findAndReplaceAll(std::string data, std::string toSearch, std::string replaceStr)
{
    size_t pos = data.find(toSearch);
    while (pos != std::string::npos)
    {
        data.replace(pos, toSearch.size(), replaceStr);
        pos = data.find(toSearch, pos + replaceStr.size());
    }

    return data;
}

std::string addIndent(std::string string, int level)
{
    std::string replace = "\n";
    for (int i = 0; i < level; i++)
    {
        replace += "\t";
    }

    return findAndReplaceAll(string, "\n", replace);
}