#pragma once

#include <vector>

#include "ICodePointProcessor.h"

namespace cppgm::pa1
{

class Utf8Processor
{
public:
    Utf8Processor(ICodePointProcessor* destProcessor);
    void process_code_unit(unsigned char codeUnit);
    void process_eof();

private:
    ICodePointProcessor* _destProcessor;
    size_t _octetCount;
    std::vector<unsigned char> _codeUnits;
};

}