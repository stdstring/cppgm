#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "IUtf32Processor.h"
#include "Utf32Processors.h"

// trigraphs sequences:
// "??="  -->  "#"
// "??/"  -->  "\"
// "??'"  -->  "^"
// "??("  -->  "["
// "??)"  -->  "]"
// "??!"  -->  "|"
// "??<"  -->  "{"
// "??>"  -->  "}"
// "??-"  -->  "~"

namespace
{

const std::unordered_map<char, char> trigraphMap = {{'=', '#'}, {'/', '\\'}, {'\'', '^'}, {'(', '['}, {')', ']'}, {'!', '|'}, {'<', '{'}, {'>', '}'}, {'-', '~'}};

constexpr char trigraphPrefixChar = '?';

}

cppgm::TrigraphProcessor::TrigraphProcessor(IUtf32Processor &consumer) : _consumer(consumer), _nextChar(TrigraphNextChar::QuestionFirstChar)
{
}

void cppgm::TrigraphProcessor::process(int32_t ch)
{
    switch (_nextChar)
    {
    case TrigraphNextChar::QuestionFirstChar:
        process_first_state(ch);
        break;
    case TrigraphNextChar::QuestionSecondChar:
        process_second_state(ch);
        break;
    case TrigraphNextChar::ThirdChar:
        process_third_state(ch);
        break;
    default:
        break;
    }
}

void cppgm::TrigraphProcessor::process_eof()
{
    send_buffer();
    _consumer.process_eof();
}

void cppgm::TrigraphProcessor::process_first_state(int32_t ch)
{
    if (ch == trigraphPrefixChar)
        _nextChar = TrigraphNextChar::QuestionSecondChar;
    else
        send_buffer(ch);
}

void cppgm::TrigraphProcessor::process_second_state(int32_t ch)
{
    if (ch == trigraphPrefixChar)
        _nextChar = TrigraphNextChar::ThirdChar;
    else
    {
        send_buffer(ch);
        _nextChar = TrigraphNextChar::QuestionFirstChar;
    }
}

void cppgm::TrigraphProcessor::process_third_state(int32_t ch)
{
    if (ch == trigraphPrefixChar)
    {
        _consumer.process(ch);
        return;
    }
    if (trigraphMap.count(ch) == 1)
        _consumer.process(trigraphMap.at(ch));
    else
        send_buffer(ch);
    _nextChar = TrigraphNextChar::QuestionFirstChar;
}

void cppgm::TrigraphProcessor::send_buffer(int32_t ch)
{
    switch (_nextChar)
    {
    case TrigraphNextChar::QuestionSecondChar:
        _consumer.process(trigraphPrefixChar);
        break;
    case TrigraphNextChar::ThirdChar:
        _consumer.process(trigraphPrefixChar);
        _consumer.process(trigraphPrefixChar);
        break;
    default:
        break;
    }
    if (ch != 0)
        _consumer.process(ch);
}

// universal-character-names: escape sequences `\uXXXX` and `\UXXXXXXXX`

namespace
{

int32_t hex_to_int(int32_t ch)
{
    if ('0' <= ch && ch <= '9')
        return ch - '0';
    if ('A' <= ch && ch <= 'F')
        return 10 + ch - 'A';
    if ('a' <= ch && ch <= 'f')
        return 10 + ch - 'a';
    return 0;
}

int32_t hex_to_int(std::vector<int32_t>::const_iterator const &start, std::vector<int32_t>::const_iterator const &end)
{
    int32_t result = 0;
    for (std::vector<int32_t>::const_iterator iterator = start; iterator != end; ++iterator)
        result = 16 * result + hex_to_int(*iterator);
    return result;
}

}

cppgm::UniversalCharProcessor::UniversalCharProcessor(IUtf32Processor &consumer) : _consumer(consumer)
{
}

void cppgm::UniversalCharProcessor::process(int32_t ch)
{
    switch (_nextChar)
    {
    case UniversalCharNextChar::BackslashChar:
        process_backslash_state(ch);
        break;
    case UniversalCharNextChar::UChar:
        process_uchar_state(ch);
        break;
    case UniversalCharNextChar::HexChar:
        process_hexchar_state(ch);
        break;
    default:
        break;
    }
}

void cppgm::UniversalCharProcessor::process_eof()
{
    send_buffer();
    _consumer.process_eof();
}

void cppgm::UniversalCharProcessor::process_backslash_state(int32_t ch)
{
    constexpr char backslashChar = '\\';
    if (ch == backslashChar)
    {
        _buffer.push_back(ch);
        _nextChar = UniversalCharNextChar::UChar;
    }
    else
        _consumer.process(ch);
}

void cppgm::UniversalCharProcessor::process_uchar_state(int32_t ch)
{
    constexpr char smallUChar = 'u';
    constexpr char bigUChar = 'U';
    if (ch == smallUChar || ch == bigUChar)
    {
        _buffer.push_back(ch);
        _nextChar = UniversalCharNextChar::HexChar;
        _expectedBufferSize = (ch == smallUChar ? 4 : 8) + _buffer.size();
    }
    else
    {
        send_buffer();
        process(ch);
    }
}

void cppgm::UniversalCharProcessor::process_hexchar_state(int32_t ch)
{
    if (std::isxdigit(ch))
    {
        _buffer.push_back(ch);
        if (_buffer.size() == static_cast<size_t>(_expectedBufferSize))
        {
            _consumer.process(hex_to_int(_buffer.cbegin() + 2, _buffer.cend()));
            reset();
        }
    }
    else
    {
        send_buffer();
        process(ch);
    }
}

void cppgm::UniversalCharProcessor::send_buffer()
{
    for (int32_t ch : _buffer)
        _consumer.process(ch);
    reset();
}

void cppgm::UniversalCharProcessor::reset()
{
    _buffer.clear();
    _expectedBufferSize = -1;
    _nextChar = UniversalCharNextChar::BackslashChar;
}