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

template<typename TProcessorTests> void check_state(TProcessorTests &obj, std::vector<int32_t> const &expectedChars, bool expectedEof)
{
    ASSERT_EQ(expectedChars, obj.consumer.utf32Chars);
    ASSERT_EQ(expectedEof, obj.consumer.eof);
}

template<typename TProcessorTests> void process_check(TProcessorTests &obj, int32_t ch, std::vector<int32_t> const &expectedChars)
{
    obj.processor.process(ch);
    check_state(obj, expectedChars, false);
}

template<typename TProcessorTests> void process_eof_check(TProcessorTests &obj, std::vector<int32_t> const &expectedChars)
{
    obj.processor.process_eof();
    check_state(obj, expectedChars, true);
    obj.consumer.reset();
}

template<typename TProcessorTests> void process_sequence(TProcessorTests &obj, std::vector<int32_t> const &sequence, int32_t expectedChar)
{
    process_sequence(obj, sequence, std::vector<int32_t>({expectedChar}));
}

template<typename TProcessorTests> void process_sequence(TProcessorTests &obj, std::vector<int32_t> const &sequence, std::vector<int32_t> const &expectedChars)
{
    for (size_t index = 0; index < sequence.size() - 1; ++index)
        process_check(obj, sequence[index], {});
    process_check(obj, sequence.back(), expectedChars);
    obj.consumer.reset();
}

template<typename TProcessorTests> void process_sequence_eof(TProcessorTests &obj, std::vector<int32_t> const &sequence, std::vector<int32_t> const &expectedChars)
{
    for (int32_t ch : sequence)
        process_check(obj, ch, {});
    process_eof_check(obj, expectedChars);
}

class TrigraphProcessorTests : public ::testing::Test
{
public:
    TrigraphProcessorTests() : processor(TrigraphProcessor(consumer))
    {
    }

    TestUtf32Consumer consumer;
    TrigraphProcessor processor;
};

TEST_F(TrigraphProcessorTests, process_known_trigraphs)
{
    process_sequence(*this, {'?', '?', '='}, '#');
    process_sequence(*this, {'?', '?', '/'}, '\\');
    process_sequence(*this, {'?', '?', '\''}, '^');
    process_sequence(*this, {'?', '?', '('}, '[');
    process_sequence(*this, {'?', '?', ')'}, ']');
    process_sequence(*this, {'?', '?', '!'}, '|');
    process_sequence(*this, {'?', '?', '<'}, '{');
    process_sequence(*this, {'?', '?', '>'}, '}');
    process_sequence(*this, {'?', '?', '-'}, '~');
}

TEST_F(TrigraphProcessorTests, process_other)
{
    // process the following character sequence: '1'
    process_sequence(*this, {'1'}, {'1'});
    // process the following character sequence: '?', '1'
    process_sequence(*this, {'?', '1'}, {'?', '1'});
    // process the following character sequence: '?', '?', '1'
    process_sequence(*this, {'?', '?', '1'}, {'?', '?', '1'});
}

TEST_F(TrigraphProcessorTests, process_several)
{
    // process the following character sequence: '1', '2'
    process_check(*this, '1', {'1'});
    process_check(*this, '2', {'1', '2'});
    consumer.reset();
    // process the following character sequence: '1', '?', '?', '='
    process_check(*this, '1', {'1'});
    process_check(*this, '?', {'1'});
    process_check(*this, '?', {'1'});
    process_check(*this, '=', {'1', '#'});
    consumer.reset();
    // process the following character sequence: '?', '?', '?', '='
    process_check(*this, '?', {});
    process_check(*this, '?', {});
    process_check(*this, '?', {'?'});
    process_check(*this, '=', {'?', '#'});
    consumer.reset();
    // process the following character sequence: '?', '?', '=', '1'
    process_check(*this, '?', {});
    process_check(*this, '?', {});
    process_check(*this, '=', {'#'});
    process_check(*this, '1', {'#', '1'});
    consumer.reset();
    // process the following character sequence: '?', '?', '=', '?', '?', '-'
    process_check(*this, '?', {});
    process_check(*this, '?', {});
    process_check(*this, '=', {'#'});
    process_check(*this, '?', {'#'});
    process_check(*this, '?', {'#'});
    process_check(*this, '-', {'#', '~'});
}

TEST_F(TrigraphProcessorTests, process_eof)
{
    // process the following character sequence: eof
    process_eof_check(*this, {});
    // process the following character sequence: '1', eof
    process_check(*this, '1', {'1'});
    process_eof_check(*this, {'1'});
    // process the following character sequence: '?', '?', eof
    process_sequence_eof(*this, {'?', '?'}, {'?', '?'});
}

class UniversalCharProcessorTests : public ::testing::Test
{
public:
    UniversalCharProcessorTests() : processor(UniversalCharProcessor(consumer))
    {
    }

    TestUtf32Consumer consumer;
    UniversalCharProcessor processor;
};

TEST_F(UniversalCharProcessorTests, process)
{
    // escape sequence `\uXXXX`
    process_sequence(*this, {'\\', 'u', '0', '0', '3', 'F'}, '?');
    process_sequence(*this, {'\\', 'u', '0', '0', '7', 'E'}, '~');
    process_sequence(*this, {'\\', 'u', '0', '0', '7', 'F'}, 0x007F);
    process_sequence(*this, {'\\', 'u', '0', '1', 'B', 'F'}, 0x01BF);
    process_sequence(*this, {'\\', 'u', '0', '8', '0', 'B'}, 0x080B);
    process_sequence(*this, {'\\', 'u', '0', 'E', 'D', 'C'}, 0x0EDC);
    process_sequence(*this, {'\\', 'u', '1', '0', 'F', '3'}, 0x10F3);
    process_sequence(*this, {'\\', 'u', '2', 'A', 'C', 'A'}, 0x2ACA);
    process_sequence(*this, {'\\', 'u', 'A', 'B', 'C', 'D'}, 0xABCD);
    // escape sequences `\UXXXXXXXX`
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '3', 'F'}, '?');
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'E'}, '~');
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'F'}, 0x0000007F);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '1', 'B', 'F'}, 0x000001BF);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '8', '0', 'B'}, 0x0000080B);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', 'E', 'D', 'C'}, 0x00000EDC);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '1', '0', 'F', '3'}, 0x000010F3);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '2', 'A', 'C', 'A'}, 0x00002ACA);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', 'A', 'B', 'C', 'D'}, 0x0000ABCD);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '1', '0', '0', 'A', 'E'}, 0x000100AE);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '3', '4', 'A', '5', '6'}, 0x00034A56);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', 'A', 'B', 'C', 'D', 'E'}, 0x000ABCDE);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '0', '0', '0', 'A'}, 0x0010000A);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '1', '2', '3', 'B'}, 0x0010123B);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', 'A', 'B', 'C', 'D'}, 0x0010ABCD);
}

TEST_F(UniversalCharProcessorTests, process_lower_case)
{
    // escape sequence `\uXXXX`
    process_sequence(*this, {'\\', 'u', '0', '0', '3', 'f'}, '?');
    process_sequence(*this, {'\\', 'u', '0', '0', '7', 'e'}, '~');
    process_sequence(*this, {'\\', 'u', '0', '0', '7', 'f'}, 0x007F);
    process_sequence(*this, {'\\', 'u', '0', '1', 'b', 'f'}, 0x01BF);
    process_sequence(*this, {'\\', 'u', '0', '8', '0', 'b'}, 0x080B);
    process_sequence(*this, {'\\', 'u', '0', 'e', 'd', 'c'}, 0x0EDC);
    process_sequence(*this, {'\\', 'u', '1', '0', 'f', '3'}, 0x10F3);
    process_sequence(*this, {'\\', 'u', '2', 'a', 'c', 'a'}, 0x2ACA);
    process_sequence(*this, {'\\', 'u', 'a', 'b', 'c', 'd'}, 0xABCD);
    // escape sequences `\UXXXXXXXX`
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '3', 'f'}, '?');
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'e'}, '~');
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'f'}, 0x0000007F);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '1', 'b', 'f'}, 0x000001BF);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', '8', '0', 'b'}, 0x0000080B);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '0', 'e', 'd', 'c'}, 0x00000EDC);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '1', '0', 'f', '3'}, 0x000010F3);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', '2', 'a', 'c', 'a'}, 0x00002ACA);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '0', 'a', 'b', 'c', 'd'}, 0x0000ABCD);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '1', '0', '0', 'a', 'e'}, 0x000100AE);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', '3', '4', 'a', '5', '6'}, 0x00034A56);
    process_sequence(*this, {'\\', 'U', '0', '0', '0', 'a', 'b', 'c', 'd', 'e'}, 0x000ABCDE);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '0', '0', '0', 'a'}, 0x0010000A);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '1', '2', '3', 'b'}, 0x0010123B);
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', 'a', 'b', 'c', 'd'}, 0x0010ABCD);
}

TEST_F(UniversalCharProcessorTests, process_other)
{
    // process the following character sequence: '1'
    process_sequence(*this, {'1'}, {'1'});
    // process the following character sequence: '\\', '1'
    process_sequence(*this, {'\\', '1'}, {'\\', '1'});
    // process the following character sequence: '\\', 'u', 'x'
    process_sequence(*this, {'\\', 'u', 'x'}, {'\\', 'u', 'x'});
    // process the following character sequence: '\\', 'u', '1', 'x'
    process_sequence(*this, {'\\', 'u', '1', 'x'}, {'\\', 'u', '1', 'x'});
    // process the following character sequence: '\\', 'u', '1', '2', 'x'
    process_sequence(*this, {'\\', 'u', '1', '2', 'x'}, {'\\', 'u', '1', '2', 'x'});
    // process the following character sequence: '\\', 'u', '1', '2', '3', 'x'
    process_sequence(*this, {'\\', 'u', '1', '2', '3', 'x'}, {'\\', 'u', '1', '2', '3', 'x'});
    // process the following character sequence: '\\', 'U', 'x'
    process_sequence(*this, {'\\', 'U', 'x'}, {'\\', 'U', 'x'});
    // process the following character sequence: '\\', 'U', '0', 'x'
    process_sequence(*this, {'\\', 'U', '0', 'x'}, {'\\', 'U', '0', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', 'x'}, {'\\', 'U', '0', '0', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', '1', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', '1', 'x'}, {'\\', 'U', '0', '0', '1', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', '1', '0', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', 'x'}, {'\\', 'U', '0', '0', '1', '0', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', '1', '0', '0', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '0', 'x'}, {'\\', 'U', '0', '0', '1', '0', '0', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', '1', '0', '0', '3', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '0', '3', 'x'}, {'\\', 'U', '0', '0', '1', '0', '0', '3', 'x'});
    // process the following character sequence: '\\', 'U', '0', '0', '1', '0', '0', '3', 'A', 'x'
    process_sequence(*this, {'\\', 'U', '0', '0', '1', '0', '0', '3', 'A', 'x'}, {'\\', 'U', '0', '0', '1', '0', '0', '3', 'A', 'x'});
}

TEST_F(UniversalCharProcessorTests, process_several)
{
    // process the following character sequence: '1', '2'
    process_check(*this, '1', {'1'});
    process_check(*this, '2', {'1', '2'});
    consumer.reset();
    // process the following character sequence: '1', '\\', 'u', '0', '0', '7', 'e'
    process_check(*this, '1', {'1'});
    process_check(*this, '\\', {'1'});
    process_check(*this, 'u', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '7', {'1'});
    process_check(*this, 'e', {'1', '~'});
    consumer.reset();
    // process the following character sequence: '1', '\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'e'
    process_check(*this, '1', {'1'});
    process_check(*this, '\\', {'1'});
    process_check(*this, 'U', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '0', {'1'});
    process_check(*this, '7', {'1'});
    process_check(*this, 'e', {'1', '~'});
    consumer.reset();
    // process the following character sequence: '\\', 'u', '0', '0', '7', 'e', '1'
    process_check(*this, '\\', {});
    process_check(*this, 'u', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '7', {});
    process_check(*this, 'e', {'~'});
    process_check(*this, '1', {'~', '1'});
    consumer.reset();
    // process the following character sequence: '\\', 'U', '0', '0', '0', '0', '0', '0', '7', 'e', '1'
    process_check(*this, '\\', {});
    process_check(*this, 'U', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '7', {});
    process_check(*this, 'e', {'~'});
    process_check(*this, '1', {'~', '1'});
    consumer.reset();
    // process the following character sequence: '\\', 'u', '0', '0', '7', 'e', '\\', 'u', '0', '0', '3', 'f'
    process_check(*this, '\\', {});
    process_check(*this, 'u', {});
    process_check(*this, '0', {});
    process_check(*this, '0', {});
    process_check(*this, '7', {});
    process_check(*this, 'e', {'~'});
    process_check(*this, '\\', {'~'});
    process_check(*this, 'u', {'~'});
    process_check(*this, '0', {'~'});
    process_check(*this, '0', {'~'});
    process_check(*this, '3', {'~'});
    process_check(*this, 'f', {'~', '?'});
}

TEST_F(UniversalCharProcessorTests, process_eof)
{
    // process the following character sequence: eof
    process_eof_check(*this, {});
    // process the following character sequence: '1', eof
    process_check(*this, '1', {'1'});
    process_eof_check(*this, {'1'});
    // process the following character sequence: '\\', eof
    process_sequence_eof(*this, {'\\'}, {'\\'});
    // process the following character sequence: '\\', 'u', eof
    process_sequence_eof(*this, {'\\', 'u'}, {'\\', 'u'});
    // process the following character sequence: '\\', 'u', '0', eof
    process_sequence_eof(*this, {'\\', 'u', '0'}, {'\\', 'u', '0'});
    // process the following character sequence: '\\', 'u', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'u', '0', '0'}, {'\\', 'u', '0', '0'});
    // process the following character sequence: '\\', 'u', '0', '0', '7', eof
    process_sequence_eof(*this, {'\\', 'u', '0', '0', '7'}, {'\\', 'u', '0', '0', '7'});
    // process the following character sequence: '\\', 'U', eof
    process_sequence_eof(*this, {'\\', 'U'}, {'\\', 'U'});
    // process the following character sequence: '\\', 'U', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0'}, {'\\', 'U', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0'}, {'\\', 'U', '0', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0', '0'}, {'\\', 'U', '0', '0', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0', '0', '0'}, {'\\', 'U', '0', '0', '0', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', '0', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0', '0', '0', '0'}, {'\\', 'U', '0', '0', '0', '0', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', '0', '0', '0', '0', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0'}, {'\\', 'U', '0', '0', '0', '0', '0', '0'});
    // process the following character sequence: '\\', 'U', '0', '0', '0', '0', '0', '0', '7', eof
    process_sequence_eof(*this, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7'}, {'\\', 'U', '0', '0', '0', '0', '0', '0', '7'});
}

}