#include "JackTokenizer.h"
#include <cctype>
#include <iostream>

#include "definitions.h"
#include <cassert>
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

            // Skip if line starts with *
            std::string tmp = m_line;
            std::remove_if(tmp.begin(), tmp.end(), isspace);
            if (tmp.rfind("*", 0) == 0)
            {
                m_line_it = m_line.end();
                continue;
            }

            // Remove comment section from command, by just ignoring the entire line.
            if (m_line.find("//") != std::string::npos ||
                m_line.find("/*") != std::string::npos ||
                m_line.find("*/") != std::string::npos)
            {
                m_line = m_line.substr(0, m_line.find("//"));
                m_line = m_line.substr(0, m_line.find("/*"));
                m_line = m_line.substr(0, m_line.find("//"));
            }

            // Remove the \r token.
            if (m_line.find("\r") != std::string::npos)
            {
                m_line.erase(m_line.find("\r"), std::string{"\r"}.length());
            }
            if (m_line.find("\t") != std::string::npos)
            {
                m_line.erase(m_line.find("\t"), std::string{"\t"}.length());
            }

            // Set the line it to the beginning.
            m_line_it = m_line.begin();

            // Skip empty lines and pure comments
            if ((m_line.size() < 1))
            {
                m_line_it = m_line.end();
                continue;
            }
        }

        // Can the token be found as a single character, e.g. '=' ?
        if (std::find(tokens.begin(), tokens.end(), std::string{*m_line_it}) !=
            tokens.end())
        {
            m_prev_token = m_token;
            m_token = *m_line_it;
            m_line_it++;
            m_is_string_const = false;
            is_token = true;
            // std::cout << m_token << std::endl;
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

        // Handle the case for when the current subtring contains a string constant
        auto quote_pos = cur_substr.find("\"");
        if (quote_pos < token_end_pos)
        {
            // If quote before space.
            token_end_pos = quote_pos - 1;
        }
        if (quote_pos == 0)
        {
            // Find the second quote.
            auto string_end_pos = cur_substr.rfind("\"");

            // Return string const token
            m_prev_token = m_token;
            m_token = std::string(m_line_it, m_line_it + string_end_pos + 1);
            is_token = true;
            m_line_it += string_end_pos + 1;
            m_is_string_const = true;
            // std::cout << m_token << std::endl;
            break;
        }

        if (token_end_pos != std::string::npos)
        {
            // Remove any 'single char tokens' from the end and any whitespace in between
            while (true)
            {
                auto end_char = std::string{*(m_line_it + token_end_pos)};

                // Sepecial handling for string constants, i.e. quotation marks.
                if (end_char == "\"")
                {
                    auto quote_pos = cur_substr.find(end_char);
                    token_end_pos = token_end_pos - (cur_substr.length() - quote_pos);
                }

                auto known_token_pos = std::find(tokens.begin(), tokens.end(), end_char);
                auto is_known_token = known_token_pos != tokens.end();
                auto is_space = end_char == " ";
                auto is_not_empty = token_end_pos >= 1;

                // Then reduce the token end_pos
                if ((is_known_token || is_space) && is_not_empty)
                    token_end_pos--;
                else
                    break;
            }

            // Get the token
            auto maybe_token = std::string(m_line_it, m_line_it + token_end_pos + 1);

            // Handle case for when there is still a special character in between
            for (auto &c : maybe_token)
            {
                auto special_char =
                    std::find(tokens.begin(), tokens.end(), std::string{c});
                if (special_char != tokens.end())
                {
                    token_end_pos = maybe_token.find(std::string{*special_char}) - 1;
                    break;
                }
            }
            m_prev_token = m_token;
            m_token = std::string(m_line_it, m_line_it + token_end_pos + 1);
            m_is_string_const = false;
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
            // Now there might still be the case that there are special fucking symbols
            // hidden somwhere. So find them and eleminate them!!!!!!!
            auto maybe_token = std::string(m_line_it, m_line_it + token_length);
            for (auto &c : maybe_token)
            {
                auto special_char =
                    std::find(tokens.begin(), tokens.end(), std::string{c});
                if (special_char != tokens.end())
                {
                    token_length = maybe_token.find(std::string{*special_char});
                    break;
                }
            }

            m_prev_token = m_token;
            m_token = std::string(m_line_it, m_line_it + token_length);
            m_is_string_const = false;
            is_token = true;
            m_line_it += token_length;
        }

        // std::cout << m_token << std::endl;
    }
};

auto JackTokenizer::tokenType() -> TokenType
{
    if (std::find(keywords.begin(), keywords.end(), m_token) != keywords.end())
        m_token_type = TokenType::KEYWORD;
    else if (std::find(symbols.begin(), symbols.end(), m_token) != symbols.end())
        m_token_type = TokenType::SYMBOL;
    else if (isdigit(m_token[0]))
        m_token_type = TokenType::INT_CONST;
    else if (m_is_string_const)
        m_token_type = TokenType::STRING_CONST;
    else
        m_token_type = TokenType::IDENTIFIER;
    return m_token_type;
};

auto JackTokenizer::prevTokenType() -> TokenType
{
    if (std::find(keywords.begin(), keywords.end(), m_prev_token) != keywords.end())
        m_prev_token_type = TokenType::KEYWORD;
    else if (std::find(symbols.begin(), symbols.end(), m_prev_token) != symbols.end())
        m_prev_token_type = TokenType::SYMBOL;
    else if (isdigit(m_prev_token[0]))
        m_prev_token_type = TokenType::INT_CONST;
    else if (m_is_string_const)
        m_prev_token_type = TokenType::STRING_CONST;
    else
        m_prev_token_type = TokenType::IDENTIFIER;
    return m_prev_token_type;
};

auto JackTokenizer::keyWord() -> KeyWord
{
    assert(m_token_type == TokenType::KEYWORD);
    return KEYWORD_MAP[m_token];
};

auto JackTokenizer::symbol() -> std::string
{
    assert(m_token_type == TokenType::SYMBOL);
    return m_token;
};

auto JackTokenizer::identifier() -> std::string
{
    assert(m_token_type == TokenType::IDENTIFIER);
    return m_token;
};

auto JackTokenizer::intVal() -> int
{
    assert(m_token_type == TokenType::INT_CONST);
    return std::atoi(m_token.c_str());
};

auto JackTokenizer::stringVal() -> std::string
{
    assert(m_token_type == TokenType::STRING_CONST);
    return m_token;
};