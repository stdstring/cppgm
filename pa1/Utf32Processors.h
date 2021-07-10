#pragma once

#include <vector>

#include "IUtf32Processor.h"

namespace cppgm
{

// TODO (std_string) : think about location
enum class TrigraphNextChar { QuestionFirstChar, QuestionSecondChar, ThirdChar };

class TrigraphProcessor : public IUtf32Processor
{
public:
    TrigraphProcessor(IUtf32Processor &consumer);

    virtual void process(int32_t ch) override;
    virtual void process_eof() override;

private:
    void process_first_state(int32_t ch);
    void process_second_state(int32_t ch);
    void process_third_state(int32_t ch);

    void send_buffer(int32_t ch = 0);

    IUtf32Processor &_consumer;
    TrigraphNextChar _nextChar;
};

// TODO (std_string) : think about location
enum class UniversalCharNextChar { BackslashChar, UChar, HexChar };

class UniversalCharProcessor : public IUtf32Processor
{
public:
    UniversalCharProcessor(IUtf32Processor &consumer);

    virtual void process(int32_t ch) override;
    virtual void process_eof() override;

private:
    void process_backslash_state(int32_t ch);
    void process_uchar_state(int32_t ch);
    void process_hexchar_state(int32_t ch);

    void send_buffer();
    void reset();

    IUtf32Processor &_consumer;
    std::vector<int32_t> _buffer;
    UniversalCharNextChar _nextChar;
    int _expectedBufferSize;
};

}