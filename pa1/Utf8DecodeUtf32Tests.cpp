#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Utf8.h"

namespace
{

struct DecodeUtf32Data
{
public:
    int Source;
    std::vector<unsigned char> Dest;
    std::string Name;
};

}

namespace cppgm
{

class Utf8DecodeUtf32Tests : public testing::TestWithParam<DecodeUtf32Data>
{
};

TEST_P(Utf8DecodeUtf32Tests, DecodeUtf32)
{
    DecodeUtf32Data data = GetParam();
    EXPECT_EQ(data.Dest, decode_utf32(data.Source));
}

TEST(Utf8BadDecodeUtf32Tests, BadDecodeUtf32)
{
    EXPECT_THROW(decode_utf32(0X110000), std::logic_error);
}

INSTANTIATE_TEST_SUITE_P(Utf8DecodeUtf32, Utf8DecodeUtf32Tests, testing::Values(DecodeUtf32Data {'?', {'?'}, "Processing_0x3F"},
                                                                                DecodeUtf32Data {'~', {'~'}, "Processing_0x7E"},
                                                                                DecodeUtf32Data {0x7F, {0x7F}, "Processing_0x7F"},
                                                                                DecodeUtf32Data {0x80, {0xC2, 0x80}, "Processing_0x80"},
                                                                                DecodeUtf32Data {0xA1, {0xC2, 0xA1}, "Processing_0xA1"},
                                                                                DecodeUtf32Data {0xC0, {0xC3, 0x80}, "Processing_0xC0"},
                                                                                DecodeUtf32Data {0x1BF, {0xC6, 0xBF}, "Processing_0x1BF"},
                                                                                DecodeUtf32Data {0x3FF, {0xCF, 0xBF}, "Processing_0x3FF"},
                                                                                DecodeUtf32Data {0x7FE, {0xDF, 0xBE}, "Processing_0x7FE"},
                                                                                DecodeUtf32Data {0x7FF, {0xDF, 0xBF}, "Processing_0x7FF"},
                                                                                DecodeUtf32Data {0x800, {0xE0, 0xA0, 0x80}, "Processing_0x800"},
                                                                                DecodeUtf32Data {0x801, {0xE0, 0xA0, 0x81}, "Processing_0x801"},
                                                                                DecodeUtf32Data {0x8A1, {0xE0, 0xA2, 0xA1}, "Processing_0x8A1"},
                                                                                DecodeUtf32Data {0xEDC, {0xE0, 0xBB, 0x9C}, "Processing_0xEDC"},
                                                                                DecodeUtf32Data {0x1000, {0xE1, 0x80, 0x80}, "Processing_0x1000"},
                                                                                DecodeUtf32Data {0x10F3, {0xE1, 0x83, 0xB3}, "Processing_0x10F3"},
                                                                                DecodeUtf32Data {0xFFFE, {0xEF, 0xBF, 0xBE}, "Processing_0xFFFE"},
                                                                                DecodeUtf32Data {0xFFFF, {0xEF, 0xBF, 0xBF}, "Processing_0xFFFF"},
                                                                                DecodeUtf32Data {0x10000, {0xF0, 0x90, 0x80, 0x80}, "Processing_0x10000"},
                                                                                DecodeUtf32Data {0x10001, {0xF0, 0x90, 0x80, 0x81}, "Processing_0x10001"},
                                                                                DecodeUtf32Data {0x118F2, {0xF0, 0x91, 0xA3, 0xB2}, "Processing_0x118F2"},
                                                                                DecodeUtf32Data {0xE0023, {0xF3, 0xA0, 0x80, 0xA3}, "Processing_0xE0023"},
                                                                                DecodeUtf32Data {0x100000, {0xF4, 0x80, 0x80, 0x80}, "Processing_0x100000"},
                                                                                DecodeUtf32Data {0x100364, {0xF4, 0x80, 0x8D, 0xA4}, "Processing_0x100364"}),
                                                                [](testing::TestParamInfo<DecodeUtf32Data> const &data){ return data.param.Name; });
}