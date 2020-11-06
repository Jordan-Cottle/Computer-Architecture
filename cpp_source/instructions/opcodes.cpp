#include "opcodes.h"

std::unordered_map<std::string, uint32_t> OPCODES = {
    {"lb", 0b00000000000000000000000000000011},
    {"fence", 0b00000000000000000000000000001111},
    {"addi", 0b00000000000000000000000000010011},
    {"auipc", 0b00000000000000000000000000010111},
    {"sb", 0b00000000000000000000000000100011},
    {"add", 0b00000000000000000000000000110011},
    {"lui", 0b00000000000000000000000000110111},
    {"fmadd.s", 0b00000000000000000000000001000011},
    {"fmsub.s", 0b00000000000000000000000001000111},
    {"fnmsub.s", 0b00000000000000000000000001001011},
    {"fnmadd.s", 0b00000000000000000000000001001111},
    {"fadd.s", 0b00000000000000000000000001010011},
    {"beq", 0b00000000000000000000000001100011},
    {"jalr", 0b00000000000000000000000001100111},
    {"jal", 0b00000000000000000000000001101111},
    {"lh", 0b00000000000000000001000000000011},
    {"fence.i", 0b00000000000000000001000000001111},
    {"slli", 0b00000000000000000001000000010011},
    {"sh", 0b00000000000000000001000000100011},
    {"sll", 0b00000000000000000001000000110011},
    {"bne", 0b00000000000000000001000001100011},
    {"lw", 0b00000000000000000010000000000011},
    {"flw", 0b00000000000000000010000000000111},
    {"slti", 0b00000000000000000010000000010011},
    {"sw", 0b00000000000000000010000000100011},
    {"fsw", 0b00000000000000000010000000100111},
    {"slt", 0b00000000000000000010000000110011},
    {"sltiu", 0b00000000000000000011000000010011},
    {"sltu", 0b00000000000000000011000000110011},
    {"lbu", 0b00000000000000000100000000000011},
    {"xori", 0b00000000000000000100000000010011},
    {"xor", 0b00000000000000000100000000110011},
    {"blt", 0b00000000000000000100000001100011},
    {"lhu", 0b00000000000000000101000000000011},
    {"srli", 0b00000000000000000101000000010011},
    {"srl", 0b00000000000000000101000000110011},
    {"bge", 0b00000000000000000101000001100011},
    {"ori", 0b00000000000000000110000000010011},
    {"or", 0b00000000000000000110000000110011},
    {"bltu", 0b00000000000000000110000001100011},
    {"andi", 0b00000000000000000111000000010011},
    {"and", 0b00000000000000000111000000110011},
    {"bgeu", 0b00000000000000000111000001100011},
    {"mul", 0b00000010000000000000000000110011},
    {"mulh", 0b00000010000000000001000000110011},
    {"mulhsu", 0b00000010000000000010000000110011},
    {"mulhu", 0b00000010000000000011000000110011},
    {"div", 0b00000010000000000100000000110011},
    {"divu", 0b00000010000000000101000000110011},
    {"rem", 0b00000010000000000110000000110011},
    {"remu", 0b00000010000000000111000000110011},
    {"fsub.s", 0b00001000000000000000000001010011},
    {"fmul.s", 0b00010000000000000000000001010011},
    {"fdiv.s", 0b00011000000000000000000001010011},
    {"fsgnj.s", 0b00100000000000000000000001010011},
    {"fsgnjn.s", 0b00100000000000000001000001010011},
    {"fsgnjx.s", 0b00100000000000000010000001010011},
    {"fmin.s", 0b00101000000000000000000001010011},
    {"fmax.s", 0b00101000000000000001000001010011},
    {"sub", 0b01000000000000000000000000110011},
    {"srai", 0b01000000000000000101000000010011},
    {"sra", 0b01000000000000000101000000110011},
    {"fsqrt.s", 0b01011000000000000000000001010011},
    {"fle.s", 0b10100000000000000000000001010011},
    {"flt.s", 0b10100000000000000001000001010011},
    {"feq.s", 0b10100000000000000010000001010011},
    {"fcvt.w.s", 0b11000000000000000000000001010011},
    {"fcvt.wu.s", 0b11000000000100000000000001010011},
    {"fcvt.s.w", 0b11010000000000000000000001010011},
    {"fcvt.s.wu", 0b11010000000100000000000001010011},
    {"fmv.x.w", 0b11100000000000000000000001010011},
    {"fclass.s", 0b11100000000000000001000001010011},
    {"fmv.w.x", 0b11110000000000000000000001010011},
};

std::unordered_map<uint32_t, std::string> INSTRUCTIONS = {
    {0b00000000000000000000000000000011, "lb"},
    {0b00000000000000000000000000001111, "fence"},
    {0b00000000000000000000000000010011, "addi"},
    {0b00000000000000000000000000010111, "auipc"},
    {0b00000000000000000000000000100011, "sb"},
    {0b00000000000000000000000000110011, "add"},
    {0b00000000000000000000000000110111, "lui"},
    {0b00000000000000000000000001000011, "fmadd.s"},
    {0b00000000000000000000000001000111, "fmsub.s"},
    {0b00000000000000000000000001001011, "fnmsub.s"},
    {0b00000000000000000000000001001111, "fnmadd.s"},
    {0b00000000000000000000000001010011, "fadd.s"},
    {0b00000000000000000000000001100011, "beq"},
    {0b00000000000000000000000001100111, "jalr"},
    {0b00000000000000000000000001101111, "jal"},
    {0b00000000000000000001000000000011, "lh"},
    {0b00000000000000000001000000001111, "fence.i"},
    {0b00000000000000000001000000010011, "slli"},
    {0b00000000000000000001000000100011, "sh"},
    {0b00000000000000000001000000110011, "sll"},
    {0b00000000000000000001000001100011, "bne"},
    {0b00000000000000000010000000000011, "lw"},
    {0b00000000000000000010000000000111, "flw"},
    {0b00000000000000000010000000010011, "slti"},
    {0b00000000000000000010000000100011, "sw"},
    {0b00000000000000000010000000100111, "fsw"},
    {0b00000000000000000010000000110011, "slt"},
    {0b00000000000000000011000000010011, "sltiu"},
    {0b00000000000000000011000000110011, "sltu"},
    {0b00000000000000000100000000000011, "lbu"},
    {0b00000000000000000100000000010011, "xori"},
    {0b00000000000000000100000000110011, "xor"},
    {0b00000000000000000100000001100011, "blt"},
    {0b00000000000000000101000000000011, "lhu"},
    {0b00000000000000000101000000010011, "srli"},
    {0b00000000000000000101000000110011, "srl"},
    {0b00000000000000000101000001100011, "bge"},
    {0b00000000000000000110000000010011, "ori"},
    {0b00000000000000000110000000110011, "or"},
    {0b00000000000000000110000001100011, "bltu"},
    {0b00000000000000000111000000010011, "andi"},
    {0b00000000000000000111000000110011, "and"},
    {0b00000000000000000111000001100011, "bgeu"},
    {0b00000010000000000000000000110011, "mul"},
    {0b00000010000000000001000000110011, "mulh"},
    {0b00000010000000000010000000110011, "mulhsu"},
    {0b00000010000000000011000000110011, "mulhu"},
    {0b00000010000000000100000000110011, "div"},
    {0b00000010000000000101000000110011, "divu"},
    {0b00000010000000000110000000110011, "rem"},
    {0b00000010000000000111000000110011, "remu"},
    {0b00001000000000000000000001010011, "fsub.s"},
    {0b00010000000000000000000001010011, "fmul.s"},
    {0b00011000000000000000000001010011, "fdiv.s"},
    {0b00100000000000000000000001010011, "fsgnj.s"},
    {0b00100000000000000001000001010011, "fsgnjn.s"},
    {0b00100000000000000010000001010011, "fsgnjx.s"},
    {0b00101000000000000000000001010011, "fmin.s"},
    {0b00101000000000000001000001010011, "fmax.s"},
    {0b01000000000000000000000000110011, "sub"},
    {0b01000000000000000101000000010011, "srai"},
    {0b01000000000000000101000000110011, "sra"},
    {0b01011000000000000000000001010011, "fsqrt.s"},
    {0b10100000000000000000000001010011, "fle.s"},
    {0b10100000000000000001000001010011, "flt.s"},
    {0b10100000000000000010000001010011, "feq.s"},
    {0b11000000000000000000000001010011, "fcvt.w.s"},
    {0b11000000000100000000000001010011, "fcvt.wu.s"},
    {0b11010000000000000000000001010011, "fcvt.s.w"},
    {0b11010000000100000000000001010011, "fcvt.s.wu"},
    {0b11100000000000000000000001010011, "fmv.x.w"},
    {0b11100000000000000001000001010011, "fclass.s"},
    {0b11110000000000000000000001010011, "fmv.w.x"},
};

std::unordered_set<uint32_t> FUNCT7_OPS = {
    0b00000000000000000000000000110011 & O_MASK,
    0b00000000000000000000000001000011 & O_MASK,
    0b00000000000000000000000001000111 & O_MASK,
    0b00000000000000000000000001001011 & O_MASK,
    0b00000000000000000000000001001111 & O_MASK,
    0b00000000000000000000000001010011 & O_MASK,
    0b00000000000000000001000000010011 & O_MASK,
    0b00000000000000000001000000110011 & O_MASK,
    0b00000000000000000010000000110011 & O_MASK,
    0b00000000000000000011000000110011 & O_MASK,
    0b00000000000000000100000000110011 & O_MASK,
    0b00000000000000000101000000010011 & O_MASK,
    0b00000000000000000101000000110011 & O_MASK,
    0b00000000000000000110000000110011 & O_MASK,
    0b00000000000000000111000000110011 & O_MASK,
    0b00000010000000000000000000110011 & O_MASK,
    0b00000010000000000001000000110011 & O_MASK,
    0b00000010000000000010000000110011 & O_MASK,
    0b00000010000000000011000000110011 & O_MASK,
    0b00000010000000000100000000110011 & O_MASK,
    0b00000010000000000101000000110011 & O_MASK,
    0b00000010000000000110000000110011 & O_MASK,
    0b00000010000000000111000000110011 & O_MASK,
    0b00001000000000000000000001010011 & O_MASK,
    0b00010000000000000000000001010011 & O_MASK,
    0b00011000000000000000000001010011 & O_MASK,
    0b00100000000000000000000001010011 & O_MASK,
    0b00100000000000000001000001010011 & O_MASK,
    0b00100000000000000010000001010011 & O_MASK,
    0b00101000000000000000000001010011 & O_MASK,
    0b00101000000000000001000001010011 & O_MASK,
    0b01000000000000000000000000110011 & O_MASK,
    0b01000000000000000101000000010011 & O_MASK,
    0b01000000000000000101000000110011 & O_MASK,
    0b01011000000000000000000001010011 & O_MASK,
    0b10100000000000000000000001010011 & O_MASK,
    0b10100000000000000001000001010011 & O_MASK,
    0b10100000000000000010000001010011 & O_MASK,
    0b11000000000000000000000001010011 & O_MASK,
    0b11000000000100000000000001010011 & O_MASK,
    0b11010000000000000000000001010011 & O_MASK,
    0b11010000000100000000000001010011 & O_MASK,
    0b11100000000000000000000001010011 & O_MASK,
    0b11100000000000000001000001010011 & O_MASK,
    0b11110000000000000000000001010011 & O_MASK,
};

std::unordered_set<uint32_t> FUNCT3_OPS = {
    0b00000000000000000000000000000011 & O_MASK,
    0b00000000000000000000000000001111 & O_MASK,
    0b00000000000000000000000000010011 & O_MASK,
    0b00000000000000000000000000100011 & O_MASK,
    0b00000000000000000000000000110011 & O_MASK,
    0b00000000000000000000000001100011 & O_MASK,
    0b00000000000000000000000001100111 & O_MASK,
    0b00000000000000000001000000000011 & O_MASK,
    0b00000000000000000001000000001111 & O_MASK,
    0b00000000000000000001000000010011 & O_MASK,
    0b00000000000000000001000000100011 & O_MASK,
    0b00000000000000000001000000110011 & O_MASK,
    0b00000000000000000001000001100011 & O_MASK,
    0b00000000000000000010000000000011 & O_MASK,
    0b00000000000000000010000000000111 & O_MASK,
    0b00000000000000000010000000010011 & O_MASK,
    0b00000000000000000010000000100011 & O_MASK,
    0b00000000000000000010000000100111 & O_MASK,
    0b00000000000000000010000000110011 & O_MASK,
    0b00000000000000000011000000010011 & O_MASK,
    0b00000000000000000011000000110011 & O_MASK,
    0b00000000000000000100000000000011 & O_MASK,
    0b00000000000000000100000000010011 & O_MASK,
    0b00000000000000000100000000110011 & O_MASK,
    0b00000000000000000100000001100011 & O_MASK,
    0b00000000000000000101000000000011 & O_MASK,
    0b00000000000000000101000000010011 & O_MASK,
    0b00000000000000000101000000110011 & O_MASK,
    0b00000000000000000101000001100011 & O_MASK,
    0b00000000000000000110000000010011 & O_MASK,
    0b00000000000000000110000000110011 & O_MASK,
    0b00000000000000000110000001100011 & O_MASK,
    0b00000000000000000111000000010011 & O_MASK,
    0b00000000000000000111000000110011 & O_MASK,
    0b00000000000000000111000001100011 & O_MASK,
    0b00000010000000000000000000110011 & O_MASK,
    0b00000010000000000001000000110011 & O_MASK,
    0b00000010000000000010000000110011 & O_MASK,
    0b00000010000000000011000000110011 & O_MASK,
    0b00000010000000000100000000110011 & O_MASK,
    0b00000010000000000101000000110011 & O_MASK,
    0b00000010000000000110000000110011 & O_MASK,
    0b00000010000000000111000000110011 & O_MASK,
    0b00100000000000000000000001010011 & O_MASK,
    0b00100000000000000001000001010011 & O_MASK,
    0b00100000000000000010000001010011 & O_MASK,
    0b00101000000000000000000001010011 & O_MASK,
    0b00101000000000000001000001010011 & O_MASK,
    0b01000000000000000000000000110011 & O_MASK,
    0b01000000000000000101000000010011 & O_MASK,
    0b01000000000000000101000000110011 & O_MASK,
    0b10100000000000000000000001010011 & O_MASK,
    0b10100000000000000001000001010011 & O_MASK,
    0b10100000000000000010000001010011 & O_MASK,
    0b11100000000000000000000001010011 & O_MASK,
    0b11100000000000000001000001010011 & O_MASK,
    0b11110000000000000000000001010011 & O_MASK,
};

std::string identify(uint32_t instruction)
{
    std::vector<uint32_t> options = {};
    uint32_t opcode = instruction & O_MASK;
    if (FUNCT3_OPS.count(opcode))
    { // opcode includes a Funct3 field
        for (auto pair : INSTRUCTIONS)
        {
            if ((pair.first & O_MASK) != opcode)
            { // Only check matching opcodes
                continue;
            }

            if ((pair.first & F3_MASK) == (instruction & F3_MASK))
            { // funct3 fields match
                options.push_back(pair.first);
            }
        }

        if (options.size() == 1)
        { // Only one func3 matched, return identified instruction
            return INSTRUCTIONS[options[0]];
        }
    }

    if (FUNCT7_OPS.count(opcode))
    { // opcode includes a Funct7 field
        for (auto option : options)
        { // funct7 is only needed when funct3 matches multiple opcodes
            if ((option & F7_MASK) == (instruction & F7_MASK))
            { // Only one funct7 should match (it's the only field left to check)
                return INSTRUCTIONS[option];
            }
        }
    }

    // No func3 or funct7 needed
    for (auto pair : INSTRUCTIONS)
    {
        if ((pair.first & O_MASK) == (instruction & O_MASK))
        {
            return pair.second;
        }
    }

    throw std::runtime_error("No keyword could be identified for " + str(instruction));
}

uint32_t singleBitMask(uint8_t index)
{
    return 0xFFFFFFFF ^ (1 << index);
}

uint32_t getBit(uint32_t data, uint8_t index)
{
    return (data & (1 << index));
}

uint32_t setBit(uint32_t data, uint8_t index, bool value)
{
    return (data & ~(1 << index)) | (value << index);
}

uint32_t slice(uint32_t data, uint8_t start, uint8_t end)
{
    uint32_t bits = 0;
    for (int i = start; i >= end; i--)
    {
        bits |= getBit(data, i);
    }

    return bits;
}

uint32_t getOpcode(uint32_t data)
{
    return data & O_MASK;
}

uint32_t getRd(uint32_t data)
{
    return (data & RD_MASK) >> 7;
}

uint32_t getR1(uint32_t data)
{
    return (data & R1_MASK) >> 15;
}

uint32_t getR2(uint32_t data)
{
    return (data & R2_MASK) >> 20;
}

uint32_t getImmediateI(uint32_t data)
{
    return (data & IMM_I_MASK) >> 20;
}

uint32_t getImmediateS(uint32_t data)
{
    // Mask and align lsb with overall lsb
    data = (data & IMM_S_MASK) >> 7;

    // Keep properly aligned lsb
    uint32_t bits = slice(data, 4, 0);

    // Align offset 11:5 with 4:0
    bits = slice(data >> 13, 11, 5) | bits;

    return bits;
}

uint32_t getImmediateSB(uint32_t data)
{
    // Start with bringing all bits together
    uint32_t bits = getImmediateS(data);

    // Unscramble the misaligned bits
    // B and C are the hex values for each bit's proper index
    uint32_t B = getBit(bits, 0);
    uint32_t C = getBit(bits, 11) >> 11;

    bits &= singleBitMask(0);    // Clear 0 index
    bits = setBit(bits, 0xB, B); // Set B into index 11
    bits = setBit(bits, 0xC, C); // Set C into index 12

    return bits;
}

uint32_t getImmediateU(uint32_t data)
{
    return data & IMM_U_MASK;
}

uint32_t getImmediateUB(uint32_t data)
{
    // 19:12 already in place
    uint32_t bits = data & 0xFF000;

    // Get value of single bits
    uint32_t lower_single = getBit(data, 20);
    uint32_t upper_single = getBit(data, 31);

    // shift 10:1 down into lsb slots
    bits |= (data & 0x7FE00000) >> 20;

    bits = setBit(bits, 20, upper_single);
    bits = setBit(bits, 11, lower_single);

    return bits;
}
