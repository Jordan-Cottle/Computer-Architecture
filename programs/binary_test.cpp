#include "test.h"
using namespace Simulation;

int main()
{
    cpu.addPipeline(&testPipeline);

    cpu.loadProgram("test_program.bin");

    std::vector<uint32_t> expected = {
        0b00000000000000001010000000000111,
        0b00000000000000000000000000110011,
        0b00000000001000000000000111010011,
        0b00000000000000000000000000110011,
        0b00000000000000000000000000110011,
        0b00000000001100001010000000100111,
        0b11111111110000001000000010010011,
        0b00000000001000001001000001100011,
    };

    for (uint32_t i = 0; i < expected.size(); i++)
    {
        uint32_t instruction = cpu.memory.read<uint32_t>(i * 4);
        RawInstruction rInstruction = RawInstruction(instruction);
        assert(instruction == expected[i]);
    }

    return 0;
}