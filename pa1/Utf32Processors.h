#pragma once

#include "IUtf32Processor.h"

namespace cppgm
{

class TrigraphProcessor : public IUtf32Processor
{
public:
    TrigraphProcessor(IUtf32Processor &consumer);

    virtual void process(int32_t ch) override;
    virtual void process_eof() override;

private:
    IUtf32Processor &_consumer;
    size_t _prefixSize;
};

}