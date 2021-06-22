#pragma once

#include <cstdint>

namespace cppgm
{

class IUtf32Processor
{
public:
    virtual void process(int32_t ch) = 0;
    virtual void process_eof() = 0;

    virtual ~IUtf32Processor() {}
};

}