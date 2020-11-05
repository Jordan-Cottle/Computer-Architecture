#include <string>
#include <unordered_map>
#include <unordered_set>

constexpr uint32_t I_MASK = 0xfd00707f;
constexpr uint32_t O_MASK = 0x7f;
constexpr uint32_t F3_MASK = 0x7000;
constexpr uint32_t F7_MASK = 0xfd000000;

std::unordered_map<std::string, uint32_t> OPCODES = {
    {"lb", 0b00000000000000000000000000000011},
    {"fmadd.s", 0b00000000000000000000000000000111},
    {"sb", 0b00000000000000000000000000001011},
    {"beq", 0b00000000000000000000000000001111},
    {"addi", 0b00000000000000000000000000010011},
    {"fadd.s", 0b00000000000000000000000000010111},
    {"add", 0b00000000000000000000000000011011},
    {"fnmsub.s", 0b00000000000000000000000000100111},
    {"jal", 0b00000000000000000000000001000011},
    {"fmsub.s", 0b00000000000000000000000001000111},
    {"jalr", 0b00000000000000000000000001001111},
    {"auipc", 0b00000000000000000000000001010011},
    {"lui", 0b00000000000000000000000001011011},
    {"fence", 0b00000000000000000000000001100011},
    {"fnmadd.s", 0b00000000000000000000000001100111},
    {"lbu", 0b00000000000000000001000000000011},
    {"blt", 0b00000000000000000001000000001111},
    {"xori", 0b00000000000000000001000000010011},
    {"xor", 0b00000000000000000001000000011011},
    {"lw", 0b00000000000000000010000000000011},
    {"sw", 0b00000000000000000010000000001011},
    {"slti", 0b00000000000000000010000000010011},
    {"slt", 0b00000000000000000010000000011011},
    {"flw", 0b00000000000000000010000001000011},
    {"fsw", 0b00000000000000000010000001001011},
    {"bltu", 0b00000000000000000011000000001111},
    {"ori", 0b00000000000000000011000000010011},
    {"or", 0b00000000000000000011000000011011},
    {"lh", 0b00000000000000000100000000000011},
    {"sh", 0b00000000000000000100000000001011},
    {"bne", 0b00000000000000000100000000001111},
    {"slli", 0b00000000000000000100000000010011},
    {"sll", 0b00000000000000000100000000011011},
    {"fence.i", 0b00000000000000000100000001100011},
    {"lhu", 0b00000000000000000101000000000011},
    {"bge", 0b00000000000000000101000000001111},
    {"srli", 0b00000000000000000101000000010011},
    {"srl", 0b00000000000000000101000000011011},
    {"sltiu", 0b00000000000000000110000000010011},
    {"sltu", 0b00000000000000000110000000011011},
    {"bgeu", 0b00000000000000000111000000001111},
    {"andi", 0b00000000000000000111000000010011},
    {"and", 0b00000000000000000111000000011011},
    {"sub", 0b00000100000000000000000000011011},
    {"sra", 0b00000100000000000101000000011011},
    {"srai", 0b00001000000000000101000000010011},
    {"fcvt.w.s", 0b00011000000000000000000000010111},
    {"fcvt.wu.s", 0b00011001000000000000000000010111},
    {"fsgnj.s", 0b00100000000000000000000000010111},
    {"fsgnjx.s", 0b00100000000000000010000000010111},
    {"fsgnjn.s", 0b00100000000000000100000000010111},
    {"fle.s", 0b00101000000000000000000000010111},
    {"feq.s", 0b00101000000000000010000000010111},
    {"flt.s", 0b00101000000000000100000000010111},
    {"fmv.x.w", 0b00111000000000000000000000010111},
    {"fclass.s", 0b00111000000000000100000000010111},
    {"fmul.s", 0b01000000000000000000000000010111},
    {"fcvt.s.w", 0b01011000000000000000000000010111},
    {"fcvt.s.wu", 0b01011001000000000000000000010111},
    {"fmv.w.x", 0b01111000000000000000000000010111},
    {"fsub.s", 0b10000000000000000000000000010111},
    {"mul", 0b10000000000000000000000000011011},
    {"div", 0b10000000000000000001000000011011},
    {"mulhsu", 0b10000000000000000010000000011011},
    {"rem", 0b10000000000000000011000000011011},
    {"mulh", 0b10000000000000000100000000011011},
    {"divu", 0b10000000000000000101000000011011},
    {"mulhu", 0b10000000000000000110000000011011},
    {"remu", 0b10000000000000000111000000011011},
    {"fmin.s", 0b10100000000000000000000000010111},
    {"fmax.s", 0b10100000000000000100000000010111},
    {"fdiv.s", 0b11000000000000000000000000010111},
    {"fsqrt.s", 0b11010000000000000000000000010111},
};

std::unordered_map<uint32_t, std::string> INSTRUCTIONS = {
    {0b00000000000000000000000000000011, "lb"},
    {0b00000000000000000000000000000111, "fmadd.s"},
    {0b00000000000000000000000000001011, "sb"},
    {0b00000000000000000000000000001111, "beq"},
    {0b00000000000000000000000000010011, "addi"},
    {0b00000000000000000000000000010111, "fadd.s"},
    {0b00000000000000000000000000011011, "add"},
    {0b00000000000000000000000000100111, "fnmsub.s"},
    {0b00000000000000000000000001000011, "jal"},
    {0b00000000000000000000000001000111, "fmsub.s"},
    {0b00000000000000000000000001001111, "jalr"},
    {0b00000000000000000000000001010011, "auipc"},
    {0b00000000000000000000000001011011, "lui"},
    {0b00000000000000000000000001100011, "fence"},
    {0b00000000000000000000000001100111, "fnmadd.s"},
    {0b00000000000000000001000000000011, "lbu"},
    {0b00000000000000000001000000001111, "blt"},
    {0b00000000000000000001000000010011, "xori"},
    {0b00000000000000000001000000011011, "xor"},
    {0b00000000000000000010000000000011, "lw"},
    {0b00000000000000000010000000001011, "sw"},
    {0b00000000000000000010000000010011, "slti"},
    {0b00000000000000000010000000011011, "slt"},
    {0b00000000000000000010000001000011, "flw"},
    {0b00000000000000000010000001001011, "fsw"},
    {0b00000000000000000011000000001111, "bltu"},
    {0b00000000000000000011000000010011, "ori"},
    {0b00000000000000000011000000011011, "or"},
    {0b00000000000000000100000000000011, "lh"},
    {0b00000000000000000100000000001011, "sh"},
    {0b00000000000000000100000000001111, "bne"},
    {0b00000000000000000100000000010011, "slli"},
    {0b00000000000000000100000000011011, "sll"},
    {0b00000000000000000100000001100011, "fence.i"},
    {0b00000000000000000101000000000011, "lhu"},
    {0b00000000000000000101000000001111, "bge"},
    {0b00000000000000000101000000010011, "srli"},
    {0b00000000000000000101000000011011, "srl"},
    {0b00000000000000000110000000010011, "sltiu"},
    {0b00000000000000000110000000011011, "sltu"},
    {0b00000000000000000111000000001111, "bgeu"},
    {0b00000000000000000111000000010011, "andi"},
    {0b00000000000000000111000000011011, "and"},
    {0b00000100000000000000000000011011, "sub"},
    {0b00000100000000000101000000011011, "sra"},
    {0b00001000000000000101000000010011, "srai"},
    {0b00011000000000000000000000010111, "fcvt.w.s"},
    {0b00011001000000000000000000010111, "fcvt.wu.s"},
    {0b00100000000000000000000000010111, "fsgnj.s"},
    {0b00100000000000000010000000010111, "fsgnjx.s"},
    {0b00100000000000000100000000010111, "fsgnjn.s"},
    {0b00101000000000000000000000010111, "fle.s"},
    {0b00101000000000000010000000010111, "feq.s"},
    {0b00101000000000000100000000010111, "flt.s"},
    {0b00111000000000000000000000010111, "fmv.x.w"},
    {0b00111000000000000100000000010111, "fclass.s"},
    {0b01000000000000000000000000010111, "fmul.s"},
    {0b01011000000000000000000000010111, "fcvt.s.w"},
    {0b01011001000000000000000000010111, "fcvt.s.wu"},
    {0b01111000000000000000000000010111, "fmv.w.x"},
    {0b10000000000000000000000000010111, "fsub.s"},
    {0b10000000000000000000000000011011, "mul"},
    {0b10000000000000000001000000011011, "div"},
    {0b10000000000000000010000000011011, "mulhsu"},
    {0b10000000000000000011000000011011, "rem"},
    {0b10000000000000000100000000011011, "mulh"},
    {0b10000000000000000101000000011011, "divu"},
    {0b10000000000000000110000000011011, "mulhu"},
    {0b10000000000000000111000000011011, "remu"},
    {0b10100000000000000000000000010111, "fmin.s"},
    {0b10100000000000000100000000010111, "fmax.s"},
    {0b11000000000000000000000000010111, "fdiv.s"},
    {0b11010000000000000000000000010111, "fsqrt.s"},
};

std::unordered_set<uint32_t> FUNCT7_OPS = {
    0b00000000000000000000000000000111,
    0b00000000000000000000000000010111,
    0b00000000000000000000000000011011,
    0b00000000000000000000000000100111,
    0b00000000000000000000000001000111,
    0b00000000000000000000000001100111,
    0b00000000000000000001000000011011,
    0b00000000000000000010000000011011,
    0b00000000000000000011000000011011,
    0b00000000000000000100000000010011,
    0b00000000000000000100000000011011,
    0b00000000000000000101000000010011,
    0b00000000000000000101000000011011,
    0b00000000000000000110000000011011,
    0b00000000000000000111000000011011,
    0b00000100000000000000000000011011,
    0b00000100000000000101000000011011,
    0b00001000000000000101000000010011,
    0b00011000000000000000000000010111,
    0b00011001000000000000000000010111,
    0b00100000000000000000000000010111,
    0b00100000000000000010000000010111,
    0b00100000000000000100000000010111,
    0b00101000000000000000000000010111,
    0b00101000000000000010000000010111,
    0b00101000000000000100000000010111,
    0b00111000000000000000000000010111,
    0b00111000000000000100000000010111,
    0b01000000000000000000000000010111,
    0b01011000000000000000000000010111,
    0b01011001000000000000000000010111,
    0b01111000000000000000000000010111,
    0b10000000000000000000000000010111,
    0b10000000000000000000000000011011,
    0b10000000000000000001000000011011,
    0b10000000000000000010000000011011,
    0b10000000000000000011000000011011,
    0b10000000000000000100000000011011,
    0b10000000000000000101000000011011,
    0b10000000000000000110000000011011,
    0b10000000000000000111000000011011,
    0b10100000000000000000000000010111,
    0b10100000000000000100000000010111,
    0b11000000000000000000000000010111,
    0b11010000000000000000000000010111,
};

std::unordered_set<uint32_t> FUNCT3_OPS = {
    0b00000000000000000000000000000011,
    0b00000000000000000000000000001011,
    0b00000000000000000000000000001111,
    0b00000000000000000000000000010011,
    0b00000000000000000000000000011011,
    0b00000000000000000000000001001111,
    0b00000000000000000000000001100011,
    0b00000000000000000001000000000011,
    0b00000000000000000001000000001111,
    0b00000000000000000001000000010011,
    0b00000000000000000001000000011011,
    0b00000000000000000010000000000011,
    0b00000000000000000010000000001011,
    0b00000000000000000010000000010011,
    0b00000000000000000010000000011011,
    0b00000000000000000010000001000011,
    0b00000000000000000010000001001011,
    0b00000000000000000011000000001111,
    0b00000000000000000011000000010011,
    0b00000000000000000011000000011011,
    0b00000000000000000100000000000011,
    0b00000000000000000100000000001011,
    0b00000000000000000100000000001111,
    0b00000000000000000100000000010011,
    0b00000000000000000100000000011011,
    0b00000000000000000100000001100011,
    0b00000000000000000101000000000011,
    0b00000000000000000101000000001111,
    0b00000000000000000101000000010011,
    0b00000000000000000101000000011011,
    0b00000000000000000110000000010011,
    0b00000000000000000110000000011011,
    0b00000000000000000111000000001111,
    0b00000000000000000111000000010011,
    0b00000000000000000111000000011011,
    0b00000100000000000000000000011011,
    0b00000100000000000101000000011011,
    0b00001000000000000101000000010011,
    0b00100000000000000000000000010111,
    0b00100000000000000010000000010111,
    0b00100000000000000100000000010111,
    0b00101000000000000000000000010111,
    0b00101000000000000010000000010111,
    0b00101000000000000100000000010111,
    0b00111000000000000000000000010111,
    0b00111000000000000100000000010111,
    0b01111000000000000000000000010111,
    0b10000000000000000000000000011011,
    0b10000000000000000001000000011011,
    0b10000000000000000010000000011011,
    0b10000000000000000011000000011011,
    0b10000000000000000100000000011011,
    0b10000000000000000101000000011011,
    0b10000000000000000110000000011011,
    0b10000000000000000111000000011011,
    0b10100000000000000000000000010111,
    0b10100000000000000100000000010111,
};