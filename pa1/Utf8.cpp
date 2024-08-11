#include <cstdint>
#include <stdexcept>
#include <vector>

#include "Utf8.h"

namespace
{
//const unsigned char singleOctetMask = 0x80;
//const unsigned char singleOctetValue = 0;
//const unsigned char otherOctetMask = 0xC0;
//const unsigned char otherOctetValue = 0x80;

bool check_octet(unsigned char octet, unsigned char mask, unsigned char expectedValue)
{
    return (octet & mask) == expectedValue;
}

bool check_utf32(int32_t utf32, int leftBorder, int rightBorder)
{
    return (leftBorder <= utf32) && (utf32 <= rightBorder);
}

}

size_t cppgm::octet_count(unsigned char first)
{
    // Char number range: 0000 0000-0000 007F - UTF-8 octet sequence: 0xxxxxxx
    if (check_octet(first, 0x80, 0))
        return 1;
    // Char number range: 0000 0080-0000 07FF - UTF-8 octet sequence: 110xxxxx 10xxxxxx
    if (check_octet(first, 0xE0, 0xC0))
        return 2;
    // Char number range: 0000 0800-0000 FFFF - UTF-8 octet sequence: 1110xxxx 10xxxxxx 10xxxxxx
    if (check_octet(first, 0xF0, 0xE0))
        return 3;
    // Char number range: 0001 0000-0010 FFFF - UTF-8 octet sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if (check_octet(first, 0xF8, 0xF0))
        return 4;
    throw std::domain_error("Bad first character");
}

int32_t cppgm::encode_utf32(unsigned char ch)
{
    if (!check_octet(ch, 0x80, 0))
        throw std::domain_error("Bad first character");
    return ch;
}

int32_t cppgm::encode_utf32(unsigned char ch1, unsigned char ch2)
{
    if (!check_octet(ch1, 0xE0, 0xC0))
        throw std::domain_error("Bad first character");
    if (!check_octet(ch2, 0xC0, 0x80))
        throw std::domain_error("Bad second character");
    return ((ch1 & 0x1F) << 6) + (ch2 & 0x3F);
}

int32_t cppgm::encode_utf32(unsigned char ch1, unsigned char ch2, unsigned char ch3)
{
    if (!check_octet(ch1, 0xF0, 0xE0))
        throw std::domain_error("Bad first character");
    if (!check_octet(ch2, 0xC0, 0x80))
        throw std::domain_error("Bad second character");
    if (!check_octet(ch3, 0xC0, 0x80))
        throw std::domain_error("Bad third character");
    return ((ch1 & 0x0F) << 12) + ((ch2 & 0x3F) << 6) + (ch3 & 0x3F);
}

int32_t cppgm::encode_utf32(unsigned char ch1, unsigned char ch2, unsigned char ch3, unsigned char ch4)
{
    if (!check_octet(ch1, 0xF8, 0xF0))
        throw std::domain_error("Bad first character");
    if (!check_octet(ch2, 0xC0, 0x80))
        throw std::domain_error("Bad second character");
    if (!check_octet(ch3, 0xC0, 0x80))
        throw std::domain_error("Bad third character");
    if (!check_octet(ch4, 0xC0, 0x80))
        throw std::domain_error("Bad fourth character");
    return ((ch1 & 0x07) << 18) + ((ch2 & 0x3F) << 12) + ((ch3 & 0x3F) << 6) + (ch4 & 0x3F);
}

int32_t cppgm::encode_utf32(std::vector<unsigned char> const &codeUnits)
{
    // TODO (std_string) : use functional style
    switch (codeUnits.size())
    {
        case 1:
            return cppgm::encode_utf32(codeUnits.at(0));
        case 2:
            return cppgm::encode_utf32(codeUnits.at(0), codeUnits.at(1));
        case 3:
            return cppgm::encode_utf32(codeUnits.at(0), codeUnits.at(1), codeUnits.at(2));
        case 4:
            return cppgm::encode_utf32(codeUnits.at(0), codeUnits.at(1), codeUnits.at(2), codeUnits.at(3));
        default:
            throw std::domain_error("Bad code units count");
    }
}

std::vector<unsigned char> cppgm::decode_utf32(int32_t ch)
{
    // Char number range: 0000 0000-0000 007F - UTF-8 octet sequence: 0xxxxxxx
    if (check_utf32(ch, 0x00000000, 0x0000007F))
    {
        unsigned char ch1 = ch & 0x7F;
        return {ch1};
    }
    // Char number range: 0000 0080-0000 07FF - UTF-8 octet sequence: 110xxxxx 10xxxxxx
    else if (check_utf32(ch, 0x00000080, 0x000007FF))
    {
        unsigned char ch1 = 0xC0 + ((ch >> 6) & 0x1F);
        unsigned char ch2 = 0x80 + (ch & 0x3F);
        return {ch1, ch2};
    }
    // Char number range: 0000 0800-0000 FFFF - UTF-8 octet sequence: 1110xxxx 10xxxxxx 10xxxxxx
    else if (check_utf32(ch, 0x00000800, 0x0000FFFF))
    {
        unsigned char ch1 = 0xE0 + ((ch >> 12) & 0x0F);
        unsigned char ch2 = 0x80 + ((ch >> 6) & 0x3F);
        unsigned char ch3 = 0x80 + (ch & 0x3F);
        return {ch1, ch2, ch3};
    }
    // Char number range: 0001 0000-0010 FFFF - UTF-8 octet sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (check_utf32(ch, 0x00010000, 0x0010FFFF))
    {
        unsigned char ch1 = 0xF0 + ((ch >> 18) & 0x07);
        unsigned char ch2 = 0x80 + ((ch >> 12) & 0x3F);
        unsigned char ch3 = 0x80 + ((ch >> 6) & 0x3F);
        unsigned char ch4 = 0x80 + (ch & 0x3F);
        return {ch1, ch2, ch3, ch4};
    }
    else
        throw std::domain_error("Bad utf32 character");
}
