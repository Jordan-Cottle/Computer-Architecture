/*
    Author: Jordan Cottle
    Created: 11/14/2020
*/

#include <stdexcept>

#include "binary.h"

uint32_t singleBitMask(uint8_t index)
{
    return FULL_MASK ^ (1 << index);
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

int twos_compliment(uint32_t data, uint8_t bit_length)
{
    int value = data;
    uint8_t bit_index = bit_length - 1;
    value += -(1 << (bit_length)) * ((int)getBit(value, bit_index) >> bit_index);
    return value;
}

uint32_t sign_extend(uint32_t data, uint8_t sign_bit_index)
{
    uint32_t sign = getBit(data, sign_bit_index) >> sign_bit_index;

    for (int i = sign_bit_index + 1; i < 32; i++)
    {
        data |= sign << i;
    }

    return data;
}

uint8_t bitLength(uint32_t data, bool assertPowerOfTwo)
{
    uint8_t length = 0;
    while (data > 0)
    {
        if (assertPowerOfTwo && data % 2 != 0 && data != 1)
        {
            throw std::runtime_error("Number checked should be a power of two!");
        }
        length += 1;
        data = data >> 1;
    }

    return length;
}