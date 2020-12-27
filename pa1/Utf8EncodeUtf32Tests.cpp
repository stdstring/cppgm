#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "Utf8.h"

namespace
{

struct EncodeUtf32Data
{
public:
    std::vector<unsigned char> Source;
    int Dest;
    std::string Name;
};

struct BadEncodeUtf32Data
{
public:
    std::vector<unsigned char> Source;
    std::string Name;
};

}

namespace cppgm
{

class Utf8EncodeUtf32Tests : public testing::TestWithParam<EncodeUtf32Data>
{
};

class Utf8BadEncodeUtf32Tests : public testing::TestWithParam<BadEncodeUtf32Data>
{
};

TEST_P(Utf8EncodeUtf32Tests, EncodeUtf32)
{
    EncodeUtf32Data data = GetParam();
    EXPECT_EQ(data.Dest, encode_utf32(data.Source));
}

TEST_P(Utf8BadEncodeUtf32Tests, BadEncodeUtf32)
{
    BadEncodeUtf32Data data = GetParam();
    EXPECT_THROW(encode_utf32(data.Source), std::domain_error);
}

INSTANTIATE_TEST_CASE_P(Utf8EncodeUtf32, Utf8EncodeUtf32Tests, testing::Values(EncodeUtf32Data {{'?'}, '?', "Processing_0x3F"},
                                                                               EncodeUtf32Data {{'~'}, '~', "Processing_0x7E"},
                                                                               EncodeUtf32Data {{0x7F}, 0x7F, "Processing_0x7F"},
                                                                               EncodeUtf32Data {{0xC2, 0x80}, 0x80, "Processing_0xC2_0x80"},
                                                                               EncodeUtf32Data {{0xC2, 0xA1}, 0xA1, "Processing_0xC2_0xA1"},
                                                                               EncodeUtf32Data {{0xC3, 0x80}, 0xC0, "Processing_0xC3_0x80"},
                                                                               EncodeUtf32Data {{0xC6, 0xBF}, 0x1BF, "Processing_0xC6_0xBF"},
                                                                               EncodeUtf32Data {{0xCF, 0xBF}, 0x3FF, "Processing_0xCF_0xBF"},
                                                                               EncodeUtf32Data {{0xDF, 0xBE}, 0x7FE, "Processing_0xDF_0xBE"},
                                                                               EncodeUtf32Data {{0xDF, 0xBF}, 0x7FF, "Processing_0xDF_0xBF"},
                                                                               EncodeUtf32Data {{0xE0, 0xA0, 0x80}, 0x800, "Processing_0xE0_0xA0_0x80"},
                                                                               EncodeUtf32Data {{0xE0, 0xA0, 0x81}, 0x801, "Processing_0xE0_0xA0_0x81"},
                                                                               EncodeUtf32Data {{0xE0, 0xA2, 0xA1}, 0x8A1, "Processing_0xE0_0xA2_0xA1"},
                                                                               EncodeUtf32Data {{0xE0, 0xBB, 0x9C}, 0xEDC, "Processing_0xE0_0xBB_0x9C"},
                                                                               EncodeUtf32Data {{0xE1, 0x80, 0x80}, 0x1000, "Processing_0xE1_0x80_0x80"},
                                                                               EncodeUtf32Data {{0xE1, 0x83, 0xB3}, 0x10F3, "Processing_0xE1_0x83_0xB3"},
                                                                               EncodeUtf32Data {{0xEF, 0xBF, 0xBE}, 0xFFFE, "Processing_0xEF_0xBF_0xBE"},
                                                                               EncodeUtf32Data {{0xEF, 0xBF, 0xBF}, 0xFFFF, "Processing_0xEF_0xBF_0xBF"},
                                                                               EncodeUtf32Data {{0xF0, 0x90, 0x80, 0x80}, 0x10000, "Processing_0xF0_0x90_0x80_0x80"},
                                                                               EncodeUtf32Data {{0xF0, 0x90, 0x80, 0x81}, 0x10001, "Processing_0xF0_0x90_0x80_0x81"},
                                                                               EncodeUtf32Data {{0xF0, 0x91, 0xA3, 0xB2}, 0x118F2, "Processing_0xF0_0x91_0xA3_0xB2"},
                                                                               EncodeUtf32Data {{0xF3, 0xA0, 0x80, 0xA3}, 0xE0023, "Processing_0xF3_0xA0_0x80_0xA3"},
                                                                               EncodeUtf32Data {{0xF4, 0x80, 0x80, 0x80}, 0x100000, "Processing_0xF4_0x80_0x80_0x80"},
                                                                               EncodeUtf32Data {{0xF4, 0x80, 0x8D, 0xA4}, 0x100364, "Processing_0xF4_0x80_0x8D_0xA4"}),
                                                               [](testing::TestParamInfo<EncodeUtf32Data> const &data){ return data.param.Name; });

INSTANTIATE_TEST_CASE_P(Utf8BadEncodeUtf32, Utf8BadEncodeUtf32Tests, testing::Values(BadEncodeUtf32Data {{0x80}, "Processing_0x80"},
                                                                                     BadEncodeUtf32Data {{0xBF, 0x80}, "Processing_0xBF_0x80"},
                                                                                     BadEncodeUtf32Data {{0xE0, 0x80}, "Processing_0xE0_0x80"},
                                                                                     BadEncodeUtf32Data {{0xC0, 0x7F}, "Processing_0xC0_0x7F"},
                                                                                     BadEncodeUtf32Data {{0xC0, 0xC0}, "Processing_0xC0_0xC0"},
                                                                                     BadEncodeUtf32Data {{0xDF, 0x80, 0x80}, "Processing_0xDF_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x80, 0x80}, "Processing_0xF0_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xE0, 0x7F, 0x80}, "Processing_0xE0_0x7F_0x80"},
                                                                                     BadEncodeUtf32Data {{0xE0, 0xC0, 0x80}, "Processing_0xE0_0xC0_0x80"},
                                                                                     BadEncodeUtf32Data {{0xE0, 0x80, 0x7F}, "Processing_0xE0_0x80_0x7F"},
                                                                                     BadEncodeUtf32Data {{0xE0, 0x80, 0xC0}, "Processing_0xE0_0x80_0xC0"},
                                                                                     BadEncodeUtf32Data {{0xEF, 0x80, 0x80, 0x80}, "Processing_0xEF_0x80_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF8, 0x80, 0x80, 0x80}, "Processing_0xF8_0x80_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x7F, 0x80, 0x80}, "Processing_0xF0_0x7F_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0xC0, 0x80, 0x80}, "Processing_0xF0_0xC0_0x80_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x80, 0x7F, 0x80}, "Processing_0xF0_0x80_0x7F_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x80, 0xC0, 0x80}, "Processing_0xF0_0x80_0xC0_0x80"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x80, 0x80, 0x7F}, "Processing_0xF0_0x80_0x80_0x7F"},
                                                                                     BadEncodeUtf32Data {{0xF0, 0x80, 0x80, 0xC0}, "Processing_0xF0_0x80_0x80_0xC0"}),
                                                                     [](testing::TestParamInfo<BadEncodeUtf32Data> const &data){ return data.param.Name; });
}