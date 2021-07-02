#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "IUtf32Processor.h"
#include "Utf32Processors.h"

namespace
{

struct TestUtf32Consumer : cppgm::IUtf32Processor
{
    TestUtf32Consumer();

    virtual void process(int32_t ch) override;
    virtual void process_eof() override;
    void reset();

    std::vector<int32_t> utf32Chars;
    bool eof;
};

TestUtf32Consumer::TestUtf32Consumer() : eof(false)
{
}

void TestUtf32Consumer::process(int32_t ch)
{
    utf32Chars.push_back(ch);
}

void TestUtf32Consumer::process_eof()
{
    eof = true;
}

void TestUtf32Consumer::reset()
{
    utf32Chars.clear();
    eof = false;
}

}

namespace cppgm
{

class TrigraphProcessorTests : public ::testing::Test
{
public:
    TrigraphProcessorTests() : trigraphProcessor(TrigraphProcessor(consumer))
    {
    }

    void check_state(std::vector<int32_t> const &expectedChars, bool expectedEof)
    {
        ASSERT_EQ(expectedChars, consumer.utf32Chars);
        ASSERT_EQ(expectedEof, consumer.eof);
    }

    void check_trigraph(std::vector<int> const &source, char expectedResult)
    {
        for (size_t index = 0; index < source.size() - 1; ++index)
        {
            trigraphProcessor.process(source[index]);
            check_state({}, false);
        }
        trigraphProcessor.process(source.back());
        check_state({expectedResult}, false);
        consumer.reset();
    }

    TestUtf32Consumer consumer;
    TrigraphProcessor trigraphProcessor;
};

TEST_F(TrigraphProcessorTests, process_known_trigraphs)
{
    check_trigraph({'?', '?', '='}, '#');
    check_trigraph({'?', '?', '/'}, '\\');
    check_trigraph({'?', '?', '\''}, '^');
    check_trigraph({'?', '?', '('}, '[');
    check_trigraph({'?', '?', ')'}, ']');
    check_trigraph({'?', '?', '!'}, '|');
    check_trigraph({'?', '?', '<'}, '{');
    check_trigraph({'?', '?', '>'}, '}');
    check_trigraph({'?', '?', '-'}, '~');
}

TEST_F(TrigraphProcessorTests, process_other)
{
    // process the following character sequence: '?', '?', '1'
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('1');
    check_state({'?', '?', '1'}, false);
    consumer.reset();
    // process the following character sequence: '?', '1'
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('1');
    check_state({'?', '1'}, false);
    consumer.reset();
    // process the following character sequence: '1'
    trigraphProcessor.process('1');
    check_state({'1'}, false);
}

TEST_F(TrigraphProcessorTests, process_several)
{
    // process the following character sequence: '1', '2'
    trigraphProcessor.process('1');
    check_state({'1'}, false);
    trigraphProcessor.process('2');
    check_state({'1', '2'}, false);
    consumer.reset();
    // process the following character sequence: '1', '?', '?', '='
    trigraphProcessor.process('1');
    check_state({'1'}, false);
    trigraphProcessor.process('?');
    check_state({'1'}, false);
    trigraphProcessor.process('?');
    check_state({'1'}, false);
    trigraphProcessor.process('=');
    check_state({'1', '#'}, false);
    consumer.reset();
    // process the following character sequence: '?', '?', '=', '1'
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('=');
    check_state({'#'}, false);
    trigraphProcessor.process('1');
    check_state({'#', '1'}, false);
    consumer.reset();
    // process the following character sequence: '?', '?', '=', '?', '?', '-'
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('=');
    check_state({'#'}, false);
    trigraphProcessor.process('?');
    check_state({'#'}, false);
    trigraphProcessor.process('?');
    check_state({'#'}, false);
    trigraphProcessor.process('-');
    check_state({'#', '~'}, false);
}

TEST_F(TrigraphProcessorTests, process_eof)
{
    // process eof without data
    trigraphProcessor.process_eof();
    check_state({}, true);
    consumer.reset();
    // process eof after the following character sequence: '1'
    trigraphProcessor.process('1');
    check_state({'1'}, false);
    trigraphProcessor.process_eof();
    check_state({'1'}, true);
    consumer.reset();
    // process eof after the following character sequence: '?', '?'
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process('?');
    check_state({}, false);
    trigraphProcessor.process_eof();
    check_state({'?', '?'}, true);
    consumer.reset();
}

}