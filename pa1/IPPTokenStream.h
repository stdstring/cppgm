#pragma once

#include <string>

struct IPPTokenStream
{
    virtual void emit_whitespace_sequence() = 0;
    virtual void emit_new_line() = 0;
    virtual void emit_header_name(std::string const &data) = 0;
    virtual void emit_identifier(std::string const &data) = 0;
    virtual void emit_pp_number(std::string const &data) = 0;
    virtual void emit_character_literal(std::string const &data) = 0;
    virtual void emit_user_defined_character_literal(std::string const &data) = 0;
    virtual void emit_string_literal(std::string const &data) = 0;
    virtual void emit_user_defined_string_literal(std::string const &data) = 0;
    virtual void emit_preprocessing_op_or_punc(std::string const &data) = 0;
    virtual void emit_non_whitespace_char(std::string const &data) = 0;
    virtual void emit_eof() = 0;

    virtual ~IPPTokenStream() {}
};
