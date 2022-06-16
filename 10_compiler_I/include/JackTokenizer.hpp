#include <fstream>
#include <iostream>
#include <string>

#include "definitions.hpp"

class JackTokenizer
{
private:
    std::fstream mInputFile;

public:
    JackTokenizer(std::string inputPath) : mInputFile(inputPath){};

    ~JackTokenizer();

    auto advance() -> void;

    auto tokenType() -> TokenType;

    auto keyWord() -> KeyWord;

    auto symbol() -> char;

    auto identifier() -> std::string;

    auto intVal() -> int;

    auto stringVal() -> std::string;
};

JackTokenizer::~JackTokenizer()
{
    mInputFile.close();
}
