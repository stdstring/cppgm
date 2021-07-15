#pragma once

#include <vector>

#include "IUtf32Processor.h"

namespace cppgm
{

enum class TokenType { Unknown = 0,
                       Whitespace,
                       NewLine,
                       HeaderName,
                       Identifier,
                       Number,
                       CharLiteral,
                       UserDefinedCharLiteral,
                       StrLiteral,
                       UserDefinedStrLiteral,
                       PreprocessingOpPunc,
                       NonWhitespaceChar };

struct Token
{
    Token(TokenType type);
    Token(TokenType type, std::vector<int32_t> const &data);

    TokenType Type;
    std::vector<int32_t> Data;
};

class ITokenParser
{
public:
    virtual void reset() = 0;
    virtual bool can_process(int32_t ch) const = 0;
    //virtual bool can_process_eof() const = 0;
    virtual void process(int32_t ch) = 0;
    //virtual void process_eof() = 0;
    virtual bool is_final_state() const = 0;
    virtual Token get_token() const = 0;
};

enum class NewLineTokenParserState { Start = 0, Final };

class NewLineTokenParser : public ITokenParser
{
public:
    NewLineTokenParser();
    virtual void reset() override;
    virtual bool can_process(int32_t ch) const override;
    virtual void process(int32_t ch) override;
    virtual bool is_final_state() const override;
    virtual Token get_token() const override;

private:
    NewLineTokenParserState _state;
};

}