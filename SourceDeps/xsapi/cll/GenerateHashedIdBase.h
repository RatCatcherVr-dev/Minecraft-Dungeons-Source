#pragma once

#include <cassert>
#include <random>
#include <string>

namespace cll
{

inline
std::string GenerateHashedIdBase()
{
    // we spend a lot of effort on generating this, because the standard
    // explicitly disallows uniform_int_distribution<uint8_t>

    static size_t const HASH_SIZE = 16;
    static char const HEX[] = "0123456789ABCDEF";

    std::random_device rd;
    std::uniform_int_distribution<uint32_t> uniform;
    static_assert(HASH_SIZE % sizeof(uint32_t) == 0, "Hashed id base size is not good");

    std::string res(HASH_SIZE * 2, '\0');

    for (auto i = 0u; i < HASH_SIZE / sizeof(uint32_t); ++i)
    {
        uint32_t v = uniform(rd);
        for (auto j = 0u; j < sizeof(uint32_t); ++j)
        {
            size_t offset = (i * sizeof(uint32_t) + j) * 2;
            assert(offset + 1 < res.size());

            size_t byteShift = j * 8;
            uint32_t byteV = (v >> byteShift) & 0xff; // get the byte we care about

            res[offset + 0] = HEX[(byteV & 0xf0) >> 4];
            res[offset + 1] = HEX[(byteV & 0x0f)];
        }
    }

    return res;
}

}
