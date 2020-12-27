#pragma once

#include <cstdint>
#include <vector>

namespace cppgm
{
    size_t octet_count(unsigned char first);
    int32_t encode_utf32(unsigned char ch);
    int32_t encode_utf32(unsigned char ch1, unsigned char ch2);
    int32_t encode_utf32(unsigned char ch1, unsigned char ch2, unsigned char ch3);
    int32_t encode_utf32(unsigned char ch1, unsigned char ch2, unsigned char ch3, unsigned char ch4);
    int32_t encode_utf32(std::vector<unsigned char> const &codeUnits);
    std::vector<unsigned char> decode_utf32(int32_t ch);
}