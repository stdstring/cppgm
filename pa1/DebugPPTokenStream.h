// (C) 2013 CPPGM Foundation. All Rights Reserved. www.cppgm.org

#pragma once

#include <iostream>
#include <string>

#include "IPPTokenStream.h"

struct DebugPPTokenStream : IPPTokenStream
{
    void emit_whitespace_sequence()
    {
        std::cout << "whitespace-sequence 0 " << std::endl;
    }

    void emit_new_line()
    {
        std::cout << "new-line 0 " << std::endl;
    }

    void emit_header_name(std::string const &data)
    {
        write_token("header-name", data);
    }

    void emit_identifier(std::string const &data)
    {
        write_token("identifier", data);
    }

    void emit_pp_number(std::string const &data)
    {
        write_token("pp-number", data);
    }

    void emit_character_literal(std::string const &data)
    {
        write_token("character-literal", data);
    }

    void emit_user_defined_character_literal(std::string const &data)
    {
        write_token("user-defined-character-literal", data);
    }

    void emit_string_literal(std::string const &data)
    {
        write_token("string-literal", data);
    }

    void emit_user_defined_string_literal(std::string const &data)
    {
        write_token("user-defined-string-literal", data);
    }

    void emit_preprocessing_op_or_punc(std::string const &data)
    {
        write_token("preprocessing-op-or-punc", data);
    }

    void emit_non_whitespace_char(std::string const &data)
    {
        write_token("non-whitespace-character", data);
    }

    void emit_eof()
    {
        std::cout << "eof" << std::endl;
    }

private:

    void write_token(std::string const &type, std::string const &data)
    {
        std::cout << type << " " << data.size() << " ";
        std::cout.write(data.data(), data.size());
        std::cout << std::endl;
    }
};
