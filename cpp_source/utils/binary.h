/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#ifndef __BINARY__
#define __BINARY__

#include <cstdint>

constexpr uint32_t FULL_MASK = 0xFFFFFFFF;

int twos_compliment(uint32_t data, uint8_t bit_length);

uint32_t sign_extend(uint32_t data, uint8_t sign_bit_index);

uint32_t singleBitMask(uint8_t index);

uint32_t getBit(uint32_t data, uint8_t index);

uint32_t setBit(uint32_t data, uint8_t index, bool value);

uint32_t slice(uint32_t data, uint8_t start, uint8_t end);

uint8_t bitLength(uint32_t data, bool assertPowerOfTwo = false);

#endif
