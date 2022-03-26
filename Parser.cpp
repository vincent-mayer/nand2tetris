#include "Parser.h"
#include <algorithm>
#include <cassert>

Parser::Parser(std::string& file_path)
{
    m_file.open(file_path);
}

auto Parser::advance() -> void
{
    std::string line;
    CommandType command_type;
    bool is_command = false;
    while (hasMoreCommands() && !is_command) {
        getline(m_file, line);

        // Remove comment section from command
        if (line.find("//") != std::string::npos) {
            line = line.substr(0, line.find("//"));
        }

        // Skip empty lines and pure comments
        if ((line.size() <= 1))
            continue;

        // Found a command, remove white spaces
        is_command = true;
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        std::cout << line << std::endl;

        if (line.find("@") == 0) {
            m_command_type = A;
        }
        else if (line.find("(") == 0) {
            m_command_type = L;
        }
        else {
            m_command_type = C;
        }
        m_command = line;
    }
}

auto Parser::symbol() -> std::string
{
    assert(m_command_type == A || m_command_type == L);
    unsigned int symbol_len;

    if (m_command.find("@") == 0) {
        symbol_len = m_command.size() - 1;
    }
    else if (m_command.find("(") == 0) {
        symbol_len = m_command.size() - 2;
    }
    else {
        std::runtime_error("Something is wrong, the command contains no '@' or '('.");
    }

    return m_command.substr(1, symbol_len);
}

auto Parser::dest() -> std::string
{
    unsigned long dest_pos = m_command.find("=");

    if (dest_pos != std::string::npos)
        return m_command.substr(0, dest_pos);
    else
        return std::string{""};
}

auto Parser::comp() -> std::string
{
    assert(m_command_type == C);
    unsigned long equal_pos     = m_command.find("=");
    unsigned long semicolon_pos = m_command.find(";");
    if (equal_pos != std::string::npos) {
        return m_command.substr(equal_pos + 1, m_command.size() - equal_pos - 1);
    }
    else if (semicolon_pos != std::string::npos) {
        return m_command.substr(0, semicolon_pos);
    }
    else {
        return "";
    }
}

auto Parser::jump() -> std::string
{
    std::string jump;
    if (m_command.find("=") != std::string::npos) {
        jump = "null";
    }
    return jump;
}
