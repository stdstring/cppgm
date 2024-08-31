#include <vector>

#include <gtest/gtest.h>

#include "ICodePointProcessor.h"
#include "Utf8Processor.h"

namespace
{

struct TestCodePointProcessor : cppgm::pa1::ICodePointProcessor
{
    TestCodePointProcessor();

    virtual void process(int32_t codePoint) override;
    virtual void process_eof() override;
    void reset();

    std::vector<int32_t> codePoints;
    bool eof;
};

TestCodePointProcessor::TestCodePointProcessor() : eof(false)
{
}

void TestCodePointProcessor::process(int32_t codePoint)
{
    codePoints.push_back(codePoint);
}

void TestCodePointProcessor::process_eof()
{
    eof = true;
}

void TestCodePointProcessor::reset()
{
    codePoints.clear();
    eof = false;
}

}

namespace cppgm::pa1
{

class Utf8ProcessorTests : public ::testing::Test
{
public:
    Utf8ProcessorTests() : utf8Processor(Utf8Processor(&codePointProcessor))
    {
    }

    void check_state(std::vector<int32_t> const &expectedChars, bool expectedEof)
    {
        ASSERT_EQ(expectedChars, codePointProcessor.codePoints);
        ASSERT_EQ(expectedEof, codePointProcessor.eof);
    }

    TestCodePointProcessor codePointProcessor;
    Utf8Processor utf8Processor;
};

TEST_F(Utf8ProcessorTests, process)
{
    // process code point, which consists of single utf8 code unit
    utf8Processor.process_code_unit('A');
    check_state({'A'}, false);
    codePointProcessor.reset();
    // process code point, which consists of 2 utf8 code units
    utf8Processor.process_code_unit(0xC3);
    check_state({}, false);
    utf8Processor.process_code_unit(0x80);
    check_state({0xC0}, false);
    codePointProcessor.reset();
    // process code point, which consists of 3 utf8 code units
    utf8Processor.process_code_unit(0xE0);
    check_state({}, false);
    utf8Processor.process_code_unit(0xBB);
    check_state({}, false);
    utf8Processor.process_code_unit(0x9C);
    check_state({0xEDC}, false);
    codePointProcessor.reset();
    // process code point, which consists of 4 utf8 code units
    utf8Processor.process_code_unit(0xF0);
    check_state({}, false);
    utf8Processor.process_code_unit(0x91);
    check_state({}, false);
    utf8Processor.process_code_unit(0xA3);
    check_state({}, false);
    utf8Processor.process_code_unit(0xB2);
    check_state({0x118F2}, false);
    codePointProcessor.reset();
    // process code point, which consists of single utf8 code unit
    // after process code point, which consists of 2 utf8 code units
    utf8Processor.process_code_unit('A');
    check_state({'A'}, false);
    utf8Processor.process_code_unit(0xC3);
    check_state({'A'}, false);
    utf8Processor.process_code_unit(0x80);
    check_state({'A', 0xC0}, false);
}

TEST_F(Utf8ProcessorTests, process_eof)
{
    // process eof without utf8 data
    utf8Processor.process_eof();
    check_state({}, true);
    codePointProcessor.reset();
    // process eof after complete utf8 code points
    utf8Processor.process_code_unit('A');
    check_state({'A'}, false);
    utf8Processor.process_eof();
    check_state({'A'}, true);
    codePointProcessor.reset();
    // process eof after incomplete utf8 code points
    utf8Processor.process_code_unit(0xC3);
    check_state({}, false);
    ASSERT_THROW(utf8Processor.process_eof(), std::logic_error);
}

}