#include <vector>

#include <gtest/gtest.h>

#include "IUtf32Processor.h"
#include "Utf8Processor.h"

namespace
{

struct TestUtf32Processor : cppgm::IUtf32Processor
{
    TestUtf32Processor();

    virtual void process(int32_t ch) override;
    virtual void process_eof() override;
    void reset();

    std::vector<int32_t> utf32Chars;
    bool eof;
};

TestUtf32Processor::TestUtf32Processor() : eof(false)
{
}

void TestUtf32Processor::process(int32_t ch)
{
    utf32Chars.push_back(ch);
}

void TestUtf32Processor::process_eof()
{
    eof = true;
}

void TestUtf32Processor::reset()
{
    utf32Chars.clear();
    eof = false;
}

}

namespace cppgm
{

class Utf8ProcessorTests : public ::testing::Test
{
public:
    Utf8ProcessorTests() : utf8Processor(Utf8Processor(testProcessor))
    {
    }

    void check_state(std::vector<int32_t> const &expectedChars, bool expectedEof)
    {
        ASSERT_EQ(expectedChars, testProcessor.utf32Chars);
        ASSERT_EQ(expectedEof, testProcessor.eof);
    }

    TestUtf32Processor testProcessor;
    Utf8Processor utf8Processor;
};

TEST_F(Utf8ProcessorTests, process)
{
    // process code point, which consists of single utf8 code unit
    utf8Processor.process('A');
    check_state({'A'}, false);
    testProcessor.reset();
    // process code point, which consists of 2 utf8 code units
    utf8Processor.process(0xC3);
    check_state({}, false);
    utf8Processor.process(0x80);
    check_state({0xC0}, false);
    testProcessor.reset();
    // process code point, which consists of 3 utf8 code units
    utf8Processor.process(0xE0);
    check_state({}, false);
    utf8Processor.process(0xBB);
    check_state({}, false);
    utf8Processor.process(0x9C);
    check_state({0xEDC}, false);
    testProcessor.reset();
    // process code point, which consists of 4 utf8 code units
    utf8Processor.process(0xF0);
    check_state({}, false);
    utf8Processor.process(0x91);
    check_state({}, false);
    utf8Processor.process(0xA3);
    check_state({}, false);
    utf8Processor.process(0xB2);
    check_state({0x118F2}, false);
    testProcessor.reset();
    // process code point, which consists of single utf8 code unit
    // after process code point, which consists of 2 utf8 code units
    utf8Processor.process('A');
    check_state({'A'}, false);
    utf8Processor.process(0xC3);
    check_state({'A'}, false);
    utf8Processor.process(0x80);
    check_state({'A', 0xC0}, false);
}

TEST_F(Utf8ProcessorTests, process_eof)
{
    // process eof without utf8 data
    utf8Processor.process_eof();
    check_state({}, true);
    testProcessor.reset();
    // process eof after complete utf8 code points
    utf8Processor.process('A');
    check_state({'A'}, false);
    utf8Processor.process_eof();
    check_state({'A'}, true);
    testProcessor.reset();
    // process eof after incomplete utf8 code points
    utf8Processor.process(0xC3);
    check_state({}, false);
    ASSERT_THROW(utf8Processor.process_eof(), std::logic_error);
}

}