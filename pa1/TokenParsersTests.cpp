#include <stdexcept>

#include <gtest/gtest.h>

#include "TokenParsers.h"

namespace
{

// LF
constexpr int32_t NewLineChar =  0x0A;

}

namespace cppgm
{

void check_token(ITokenParser const &parser, TokenType expectedType, std::vector<int32_t> const &expectedData)
{
    const Token token(parser.get_token());
    ASSERT_EQ(expectedType, token.Type);
    ASSERT_EQ(expectedData, token.Data);
}

class NewLineTokenParserTests : public ::testing::Test
{
public:
    NewLineTokenParserTests()
    {
    }

    NewLineTokenParser parser;
};

TEST_F(NewLineTokenParserTests, process)
{
    ASSERT_EQ(false, parser.is_final_state());
    ASSERT_EQ(true, parser.can_process(NewLineChar));
    ASSERT_EQ(false, parser.can_process('A'));
    ASSERT_NO_THROW(parser.process(NewLineChar));
    ASSERT_EQ(true, parser.is_final_state());
    ASSERT_EQ(false, parser.can_process(NewLineChar));
    ASSERT_EQ(false, parser.can_process('A'));
    check_token(parser, TokenType::NewLine, {});
}

TEST_F(NewLineTokenParserTests, process_with_exceptions)
{
    ASSERT_EQ(false, parser.is_final_state());
    ASSERT_THROW(parser.get_token(), std::logic_error);
    ASSERT_THROW(parser.process('A'), std::logic_error);
    ASSERT_NO_THROW(parser.process(NewLineChar));
    ASSERT_EQ(true, parser.is_final_state());
    ASSERT_THROW(parser.process('A'), std::logic_error);
    ASSERT_THROW(parser.process(NewLineChar), std::logic_error);
}

TEST_F(NewLineTokenParserTests, process_with_reset)
{
    // reset in start state
    ASSERT_EQ(false, parser.is_final_state());
    ASSERT_NO_THROW(parser.reset());
    // first attempt
    ASSERT_NO_THROW(parser.process(NewLineChar));
    ASSERT_EQ(true, parser.is_final_state());
    check_token(parser, TokenType::NewLine, {});
    ASSERT_NO_THROW(parser.reset());
    // second attempt
    ASSERT_EQ(false, parser.is_final_state());
    ASSERT_NO_THROW(parser.process(NewLineChar));
    check_token(parser, TokenType::NewLine, {});
    ASSERT_EQ(true, parser.is_final_state());
}

}