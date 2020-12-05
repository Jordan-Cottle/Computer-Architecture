
/*
    Author: Jordan Cottle
    Created: 10/02/2020
*/

#ifndef __MISC__
#define __MISC__

#include <string>
#include <iostream>

#define DEBUG          \
    if (DEBUG_ENABLED) \
    std::cout << "DEBUG -- "

#define INFO          \
    if (INFO_ENABLED) \
    std::cout << "INFO -- "

#define WARNING          \
    if (WARNING_ENABLED) \
    std::cout << "WARNING -- "

#define ERROR          \
    if (ERROR_ENABLED) \
    std::cout << "ERROR -- "

#define DEBUG_ENABLED true
#define INFO_ENABLED true
#define WARNING_ENABLED true
#define ERROR_ENABLED true

struct printable
{
    virtual std::string __str__() = 0;

    virtual ~printable(){};
};

std::string str(printable *obj);
std::string str(printable &obj);
std::string str(int num);
std::string str(uint32_t num);
std::string str(ulong num);
std::string str(float num);

std::ostream &operator<<(std::ostream &output, printable &p);
std::ostream &operator<<(std::ostream &output, printable *p);

std::string findAndReplaceAll(std::string, std::string, std::string);
std::string addIndent(std::string, int level = 1);

#endif
