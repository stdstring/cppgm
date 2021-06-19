#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Utf8.h"

namespace
{

struct OctetCountData
{
public:
    unsigned char FirstOctet;
    int Count;
    std::string Name;
};

struct BadOctetCountData
{
public:
    unsigned char FirstOctet;
    std::string Name;
};

}

namespace cppgm
{

class Utf8OctetCountTests : public testing::TestWithParam<OctetCountData>
{
};

class Utf8BadOctetCountTests : public testing::TestWithParam<BadOctetCountData>
{
};

TEST_P(Utf8OctetCountTests, OctetCount)
{
    OctetCountData data = GetParam();
    EXPECT_EQ(data.Count, octet_count(data.FirstOctet));
}

TEST_P(Utf8BadOctetCountTests, BadOctetCount)
{
    BadOctetCountData data = GetParam();
    EXPECT_THROW(octet_count(data.FirstOctet), std::domain_error);
}

INSTANTIATE_TEST_SUITE_P(Utf8OctetCount, Utf8OctetCountTests, testing::Values(OctetCountData {0x01, 1, "Processing_0x01"},
                                                                              OctetCountData {0x02, 1, "Processing_0x02"},
                                                                              OctetCountData {0x7F, 1, "Processing_0x7F"},
                                                                              OctetCountData {0xC0, 2, "Processing_0xC0"},
                                                                              OctetCountData {0xC1, 2, "Processing_0xC1"},
                                                                              OctetCountData {0xDF, 2, "Processing_0xDF"},
                                                                              OctetCountData {0xE0, 3, "Processing_0xE0"},
                                                                              OctetCountData {0xE1, 3, "Processing_0xE1"},
                                                                              OctetCountData {0xEF, 3, "Processing_0xEF"},
                                                                              OctetCountData {0xF0, 4, "Processing_0xF0"},
                                                                              OctetCountData {0xF1, 4, "Processing_0xF1"},
                                                                              OctetCountData {0xF7, 4, "Processing_0xF7"}),
                                                              [](testing::TestParamInfo<OctetCountData> const &data){ return data.param.Name; });

INSTANTIATE_TEST_SUITE_P(Utf8BadOctetCount, Utf8BadOctetCountTests, testing::Values(BadOctetCountData {0x80, "Processing_0x80"},
                                                                                    BadOctetCountData {0x81, "Processing_0x81"},
                                                                                    BadOctetCountData {0xBF, "Processing_0xBF"},
                                                                                    BadOctetCountData {0xF8, "Processing_0xF8"},
                                                                                    BadOctetCountData {0xF9, "Processing_0xF9"},
                                                                                    BadOctetCountData {0xFF, "Processing_0xFF"}),
                                                                    [](testing::TestParamInfo<BadOctetCountData> const &data){ return data.param.Name; });

}