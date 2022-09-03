#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "definitions.h"

class JackTokenizer
{
private:
    std::ifstream m_file;
    std::string m_token;
    std::string m_line;
    std::string::iterator m_line_it;
    bool m_is_string_const;
    TokenType m_token_type;

public:
    JackTokenizer(std::string input_path)
        : m_file(input_path), m_token(""), m_line(""), m_line_it(m_line.end()){};

    ~JackTokenizer()
    {
        m_file.close();
    }

    auto hasMoreTokens() -> bool
    {
        return !m_file.eof();
    }

    auto advance() -> void;

    auto tokenType() -> TokenType;

    auto keyWord() -> KeyWord;

    auto symbol() -> std::string;

    auto identifier() -> std::string;

    auto intVal() -> int;

    auto stringVal() -> std::string;

    auto getToken() -> std::string
    {
        while (m_token.find("\"") != std::string::npos)
        {
            m_token.erase(m_token.find("\""), std::string{"\""}.length());
        }
        return m_token;
    };
};
