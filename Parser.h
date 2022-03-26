#include <fstream>
#include <iostream>
#include <string>

enum CommandType { A, C, L };

class Parser
{
public:
    Parser(std::string& file_path);

    auto hasMoreCommands() -> bool { return !m_file.eof(); }

    auto commandType() -> CommandType { return m_command_type; }

    auto symbol() -> std::string;

    auto dest() -> std::string;

    auto comp() -> std::string;

    auto jump() -> std::string;

    auto advance() -> void;

    auto reset() -> void
    {
        m_file.clear();
        m_file.seekg(0);
    }

private:
    CommandType m_command_type;
    std::string m_command;
    std::ifstream m_file;
};