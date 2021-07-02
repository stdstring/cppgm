#include <stdexcept>

#include "IUtf32Processor.h"
#include "Utf8.h"

#include "Utf8Processor.h"

cppgm::Utf8Processor::Utf8Processor(IUtf32Processor &consumer) : _consumer(consumer), _octetCount(0)
{
}

void cppgm::Utf8Processor::process(unsigned char codeUnit)
{
    if (_octetCount == 0)
        _octetCount = octet_count(codeUnit);
    _codeUnits.push_back(codeUnit);
    if (_codeUnits.size() == _octetCount)
    {
        _consumer.process(encode_utf32(_codeUnits));
        _codeUnits.clear();
        _octetCount = 0;
    }
}

void cppgm::Utf8Processor::process_eof()
{
    if (!_codeUnits.empty())
        throw std::logic_error("Trying to process EOF when unprocessed code units exist");
    _consumer.process_eof();
}