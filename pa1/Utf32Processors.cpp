#include <unordered_map>
#include <iostream>

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

std::unordered_map<char, char> trigraphMap = {{'=', '#'}, {'/', '\\'}, {'\'', '^'}, {'(', '['}, {')', ']'}, {'!', '|'}, {'<', '{'}, {'>', '}'}, {'-', '~'}};

char trigraphPrefixChar = '?';

size_t trigraphPrefixSize = 2;

}

cppgm::TrigraphProcessor::TrigraphProcessor(IUtf32Processor &consumer) : _consumer(consumer), _prefixSize(0)
{
}

void cppgm::TrigraphProcessor::process(int32_t ch)
{
    if (_prefixSize < trigraphPrefixSize && ch == trigraphPrefixChar)
        ++_prefixSize;
    else if (_prefixSize == trigraphPrefixSize && trigraphMap.count(ch) == 1)
    {
        _consumer.process(trigraphMap[ch]);
        _prefixSize = 0;
    }
    else
    {
        for (size_t index = 0; index < _prefixSize; ++index)
            _consumer.process(trigraphPrefixChar);
        _consumer.process(ch);
        _prefixSize = 0;
    }
}

void cppgm::TrigraphProcessor::process_eof()
{
    for (size_t index = 0; index < _prefixSize; ++index)
        _consumer.process(trigraphPrefixChar);
    _consumer.process_eof();
    _prefixSize = 0;
}