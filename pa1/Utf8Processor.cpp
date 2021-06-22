#include <stdexcept>

#include "IUtf32Processor.h"
#include "Utf8.h"

#include "Utf8Processor.h"

cppgm::Utf8Processor::Utf8Processor(IUtf32Processor &utf32Processor) : _utf32Processor(utf32Processor), _octetCount(0)
{
}

void cppgm::Utf8Processor::process(unsigned char codeUnit)
{
    if (_octetCount == 0)
        _octetCount = octet_count(codeUnit);
    _codeUnits.push_back(codeUnit);
    if (_codeUnits.size() == _octetCount)
    {
        _utf32Processor.process(encode_utf32(_codeUnits));
        _codeUnits.clear();
        _octetCount = 0;
    }
}

void cppgm::Utf8Processor::process_eof()
{
    if (!_codeUnits.empty())
        throw std::logic_error("Trying to process EOF when unprocessed code units exist");
    _utf32Processor.process_eof();
}