#pragma once

#include <vector>

#include "IUtf32Processor.h"

namespace cppgm
{

class Utf8Processor
{
public:
    Utf8Processor(IUtf32Processor &utf32Processor);
    void process(unsigned char codeUnit);
    void process_eof();

private:
    IUtf32Processor &_utf32Processor;
    size_t _octetCount;
    std::vector<unsigned char> _codeUnits;
};

}