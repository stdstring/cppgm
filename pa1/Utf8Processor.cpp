#include <stdexcept>

#include "ICodePointProcessor.h"
#include "Utf8.h"
#include "Utf8Processor.h"

cppgm::pa1::Utf8Processor::Utf8Processor(cppgm::pa1::ICodePointProcessor* destProcessor) :
    _destProcessor(destProcessor),
    _octetCount(0)
{
}

void cppgm::pa1::Utf8Processor::process_code_unit(unsigned char codeUnit)
{
    if (_octetCount == 0)
        _octetCount = octet_count(codeUnit);
    _codeUnits.emplace_back(codeUnit);
    if (_codeUnits.size() == _octetCount)
    {
        int32_t codePoint = encode_utf32(_codeUnits);
        _destProcessor->process(codePoint);
        _octetCount = 0;
        _codeUnits.clear();
    }
}

void cppgm::pa1::Utf8Processor::process_eof()
{
    if (_octetCount > 0)
        throw std::logic_error("Bad code unit stream");
    _destProcessor->process_eof();
}