#pragma once

#include <vector>

#include "IUtf32Processor.h"

namespace cppgm
{

class Utf8Processor
{
public:
    Utf8Processor(IUtf32Processor &consumer);
    void process(unsigned char codeUnit);
    void process_eof();

private:
    IUtf32Processor &_consumer;
    size_t _octetCount;
    std::vector<unsigned char> _codeUnits;
};

}