#include <fstream>

#include "test.h"

std::vector<uint32_t> load_trace(std::string fileName)
{

    std::vector<uint32_t> accessAddresses = std::vector<uint32_t>();
    std::ifstream traceFile(fileName, std::ios::binary);

    if (!traceFile)
    {
        throw std::runtime_error("Cannot open " + fileName);
    }

    uint32_t address;
    while (traceFile.read((char *)&address, sizeof(uint32_t)))
    {
        accessAddresses.push_back(address);
    }

    return accessAddresses;
}

int main()
{
    std::vector<uint32_t> trace = load_trace("memory_trace");
    for (auto address : trace)
    {
        std::cout << address << "\n";
    }
}