#pragma once

namespace cppgm::pa1
{

class ICodePointProcessor
{
public:
    virtual void process(int32_t codePoint) = 0;
    virtual void process_eof() = 0;
    virtual ~ICodePointProcessor() {}
};

}