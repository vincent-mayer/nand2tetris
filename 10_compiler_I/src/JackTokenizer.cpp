#include <JackTokenizer.h>
#include <iostream>

#include "definitions.h"

// Approach:
// Get entire line
// traverse line character by character and check when a keyword or smth is found
// remember position in line and when to load next line
auto JackTokenizer::advance() -> void
{
    bool is_token = false;
    while (!is_token && hasMoreTokens())
    {
        if (m_line_it == m_line.end())
        {
            getline(m_file, m_line);
            m_line_it = m_line.begin();

            // Remove comment section from command and the weird end-line char
            if (m_line.find("//") != std::string::npos ||
                m_line.find("/*") != std::string::npos ||
                m_line.find("*/") != std::string::npos ||
                m_line.find("\r") != std::string::npos)
            {
                m_line = m_line.substr(0, m_line.find("//"));
                m_line = m_line.substr(0, m_line.find("/*"));
                m_line = m_line.substr(0, m_line.find("//"));
                m_line = m_line.substr(0, m_line.find("\r"));
                m_line_it = m_line.begin();
            }

            // Skip empty lines and pure comments
            if ((m_line.size() <= 1))
            {
                m_line_it = m_line.end();
                continue;
            }
        }

        // Can the token be found as a single character, e.g. '=' ?
        if (std::find(tokens.begin(), tokens.end(), std::string{*m_line_it}) !=
            tokens.end())
        {
            m_token = *m_line_it;
            m_line_it++;
            is_token = true;
            std::cout << m_token << std::endl;
            break;
        }

        // If there is still whitespace left, read characters until next space
        std::string cur_substr{m_line_it, m_line.end()};
        auto token_end_pos = cur_substr.find(" ");
        // If we have a leading white space just increase the iterator and continue
        if (token_end_pos == 0)
        {
            m_line_it++;
            continue;
        }
        if (token_end_pos != std::string::npos)
        {
            // Remove any 'single char tokens' from the end and any whitespace in between
            while (
                (std::find(tokens.begin(), tokens.end(),
                           std::string{*(m_line_it + token_end_pos)}) != tokens.end() ||
                 std::string{*(m_line_it + token_end_pos)} == " ") &&
                token_end_pos > 1)
            {
                token_end_pos--;
            }

            // Get the token
            m_token = std::string(m_line_it, m_line_it + token_end_pos + 1);
            is_token = true;
            m_line_it += token_end_pos + 1;
        }
        // Now there was no white space left, so we are at the last tokens of the line.
        // else
        else
        {
            auto token_length = cur_substr.length();
            auto token_end_it = m_line.end() - 1;
            while ((std::find(tokens.begin(), tokens.end(),
                              std::string{*(token_end_it)}) != tokens.end()) &&
                   (token_length > 0))
            {
                token_length--;
                token_end_it--;
            }
            m_token = std::string(m_line_it, m_line_it + token_length);
            is_token = true;
            m_line_it += token_length;
        }

        std::cout << m_token << std::endl;
    }
};

auto JackTokenizer::tokenType() -> TokenType{};

auto JackTokenizer::keyWord() -> KeyWord{};

auto JackTokenizer::symbol() -> char{};

auto JackTokenizer::identifier() -> std::string{};

auto JackTokenizer::intVal() -> int{};

auto JackTokenizer::stringVal() -> std::string{};