#include <JackTokenizer.h>
#include <iostream>

// Approach:
// Get entire line
// traverse line character by character and check when a keyword or smth is found
// remember position in line and when to load next line
auto JackTokenizer::advance() -> void
{
    bool is_token = false;
    while (!is_token && hasMoreTokens())
    {
        if (m_line_consumed)
            getline(m_file, m_line);

        // Remove comment section from command
        if (m_line.find("//") != std::string::npos ||
            m_line.find("/*") != std::string::npos ||
            m_line.find("*/") != std::string::npos)
        {
            m_line = m_line.substr(0, m_line.find("//"));
            m_line = m_line.substr(0, m_line.find("/*"));
            m_line = m_line.substr(0, m_line.find("//"));
        }

        // Skip empty lines and pure comments
        if ((m_line.size() <= 1))
        {
            m_line_consumed = true;
            continue;
        }

        // Found a token, remove white space.
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Safe the token
        m_token = line;
        std::cout << m_token << std::endl;
    }
};

auto JackTokenizer::tokenType() -> TokenType{};

auto JackTokenizer::keyWord() -> KeyWord{};

auto JackTokenizer::symbol() -> char{};

auto JackTokenizer::identifier() -> std::string{};

auto JackTokenizer::intVal() -> int{};

auto JackTokenizer::stringVal() -> std::string{};