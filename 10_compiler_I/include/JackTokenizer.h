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

    auto symbol() -> char;

    auto identifier() -> std::string;

    auto intVal() -> int;

    auto stringVal() -> std::string;
};
