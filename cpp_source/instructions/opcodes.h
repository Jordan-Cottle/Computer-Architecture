
#ifndef __OPCODES__
#define __OPCODES__

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "misc.h"

// Mask of all possible "identity" bits
constexpr uint32_t I_MASK = 0xfd00707f;

// Mask for just opcode bits
constexpr uint32_t O_MASK = 0x7f;

// Masks for funct3 and funct7 fields
constexpr uint32_t F3_MASK = 0x7000;
constexpr uint32_t F7_MASK = 0xfe000000;

constexpr uint32_t RD_MASK = 0xF80;
constexpr uint32_t R1_MASK = 0xF8000;
constexpr uint32_t R2_MASK = 0x1F00000;
constexpr uint32_t IMM_I_MASK = 0xFFF00000;
constexpr uint32_t IMM_S_MASK = 0xFE000F80;
constexpr uint32_t IMM_U_MASK = 0xFFFFF000;

std::string identify(uint32_t instruction);

uint32_t singleBitMask(uint8_t index);

uint32_t getBit(uint32_t data, uint8_t index);

uint32_t setBit(uint32_t data, uint8_t index, bool value);

uint32_t slice(uint32_t data, uint8_t start, uint8_t end);

uint32_t getOpcode(uint32_t data);

uint32_t getRd(uint32_t data);

uint32_t getR1(uint32_t data);

uint32_t getR2(uint32_t data);

uint32_t getImmediateI(uint32_t data);

uint32_t getImmediateS(uint32_t data);

uint32_t getImmediateSB(uint32_t data);

uint32_t getImmediateU(uint32_t data);

uint32_t getImmediateUB(uint32_t data);

#endif // __OPCODES__