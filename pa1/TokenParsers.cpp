#include <stdexcept>
#include <vector>
#include <utility>

#include "TokenParsers.h"

namespace
{

// LF
constexpr int32_t NewLineChar =  0x0A;

// See C++ standard 2.11 Identifiers and Appendix/Annex E.1
const std::vector<std::pair<int32_t, int32_t>> annexE1AllowedRanges =
{
    {0xA8,0xA8},
    {0xAA,0xAA},
    {0xAD,0xAD},
    {0xAF,0xAF},
    {0xB2,0xB5},
    {0xB7,0xBA},
    {0xBC,0xBE},
    {0xC0,0xD6},
    {0xD8,0xF6},
    {0xF8,0xFF},
    {0x100,0x167F},
    {0x1681,0x180D},
    {0x180F,0x1FFF},
    {0x200B,0x200D},
    {0x202A,0x202E},
    {0x203F,0x2040},
    {0x2054,0x2054},
    {0x2060,0x206F},
    {0x2070,0x218F},
    {0x2460,0x24FF},
    {0x2776,0x2793},
    {0x2C00,0x2DFF},
    {0x2E80,0x2FFF},
    {0x3004,0x3007},
    {0x3021,0x302F},
    {0x3031,0x303F},
    {0x3040,0xD7FF},
    {0xF900,0xFD3D},
    {0xFD40,0xFDCF},
    {0xFDF0,0xFE44},
    {0xFE47,0xFFFD},
    {0x10000,0x1FFFD},
    {0x20000,0x2FFFD},
    {0x30000,0x3FFFD},
    {0x40000,0x4FFFD},
    {0x50000,0x5FFFD},
    {0x60000,0x6FFFD},
    {0x70000,0x7FFFD},
    {0x80000,0x8FFFD},
    {0x90000,0x9FFFD},
    {0xA0000,0xAFFFD},
    {0xB0000,0xBFFFD},
    {0xC0000,0xCFFFD},
    {0xD0000,0xDFFFD},
    {0xE0000,0xEFFFD}
};

// See C++ standard 2.11 Identifiers and Appendix/Annex E.2
const std::vector<std::pair<int32_t, int32_t>> annexE2DisallowedInitiallyRanges =
{
    {0x300,0x36F},
    {0x1DC0,0x1DFF},
    {0x20D0,0x20FF},
    {0xFE20,0xFE2F}
};

}

cppgm::Token::Token(TokenType type) : Type(type)
{
}

cppgm::Token::Token(TokenType type, std::vector<int32_t> const &data) : Type(type), Data(data)
{
}

cppgm::WhitespaceTokenParser::WhitespaceTokenParser() : _state(SimpleTokenParserState::Start)
{
}

void cppgm::WhitespaceTokenParser::reset()
{
    _state = SimpleTokenParserState::Start;
}

bool cppgm::WhitespaceTokenParser::can_process(int32_t ch) const
{
    return std::isspace(ch) && ch != NewLineChar;
}

void cppgm::WhitespaceTokenParser::process(int32_t ch)
{
    if (!std::isspace(ch) || ch == NewLineChar)
        throw std::logic_error("Bad symbol");
    _state = SimpleTokenParserState::Final;
}

bool cppgm::WhitespaceTokenParser::is_final_state() const
{
    return _state == SimpleTokenParserState::Final;
}

cppgm::Token cppgm::WhitespaceTokenParser::get_token() const
{
    if (_state != SimpleTokenParserState::Final)
        throw std::logic_error("Non final state");
    return Token(TokenType::Whitespace);
}

/*cppgm::SingleLineCommentTokenParser::SingleLineCommentTokenParser()
{}

void cppgm::SingleLineCommentTokenParser::reset()
{}

bool cppgm::SingleLineCommentTokenParser::can_process(int32_t ch) const
{}

void cppgm::SingleLineCommentTokenParser::process(int32_t ch)
{}

bool cppgm::SingleLineCommentTokenParser::is_final_state() const
{}

cppgm::Token cppgm::SingleLineCommentTokenParser::get_token() const
{}


cppgm::MultiLineCommentTokenParser::MultiLineCommentTokenParser()
{}

void cppgm::MultiLineCommentTokenParser::reset()
{}

bool cppgm::MultiLineCommentTokenParser::can_process(int32_t ch) const
{}

void cppgm::MultiLineCommentTokenParser::process(int32_t ch)
{}

bool cppgm::MultiLineCommentTokenParser::is_final_state() const
{}

cppgm::Token cppgm::MultiLineCommentTokenParser::get_token() const
{}*/

cppgm::NewLineTokenParser::NewLineTokenParser() : _state(SimpleTokenParserState::Start)
{
}

void cppgm::NewLineTokenParser::reset()
{
    _state = SimpleTokenParserState::Start;
}

bool cppgm::NewLineTokenParser::can_process(int32_t ch) const
{
    return _state == SimpleTokenParserState::Start && ch == NewLineChar;
}

void cppgm::NewLineTokenParser::process(int32_t ch)
{
    switch (_state)
    {
    case SimpleTokenParserState::Start:
        if (ch != NewLineChar)
            throw std::logic_error("Bad state");
        _state = SimpleTokenParserState::Final;
        break;
    case SimpleTokenParserState::Final:
        throw std::logic_error("Bad state");
    default:
        break;
    }
}

bool cppgm::NewLineTokenParser::is_final_state() const
{
    return _state == SimpleTokenParserState::Final;
}

cppgm::Token cppgm::NewLineTokenParser::get_token() const
{
    if (_state != SimpleTokenParserState::Final)
        throw std::logic_error("Non final state");
    return Token(TokenType::NewLine);
}