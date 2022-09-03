#include "CompilationEngine.h"
#include "definitions.h"
#include <string>

auto CompilationEngine::tokenTypeToString(TokenType token) -> std::string
{
    std::string type = "";
    if (mTokenizer->tokenType() == TokenType::KEYWORD)
        type = std::string{"keyword"};
    else if (mTokenizer->tokenType() == TokenType::SYMBOL)
        type = std::string{"symbol"};
    else if (mTokenizer->tokenType() == TokenType::IDENTIFIER)
        type = std::string{"identifier"};
    else if (mTokenizer->tokenType() == TokenType::INT_CONST)
        type = std::string{"integerConstant"};
    else if (mTokenizer->tokenType() == TokenType::STRING_CONST)
        type = std::string{"stringConstant"};

    return type;
}

auto CompilationEngine::easyCompile() -> void
{
    while (true)
    {
        mTokenizer->advance();
        if (!mTokenizer->hasMoreTokens())
            return;
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }
};

auto CompilationEngine::compileClass() -> void
{
    // class token
    std::cout << "<class>\n";
    mDepth++;

    // class Main {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());

    while (mTokenizer->getToken() != std::string{"}"})
    {
        // Get the next token.
        mTokenizer->advance();

        // If the token is function, compile subroutine.
        if (mTokenizer->getToken() == std::string{"function"} ||
            mTokenizer->getToken() == std::string{"method"})
            this->compileSubroutine();

        else if (mTokenizer->getToken() == std::string{"field"} ||
                 mTokenizer->getToken() == std::string{"static"})
            this->compileClassVarDecl();
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mDepth--;

    // class
    std::cout << "</class>\n";
};

auto CompilationEngine::compileClassVarDecl() -> void
{
    // classVarDec
    std::cout << "<classVarDec>\n";
    mDepth++;

    while (mTokenizer->getToken() != std::string{";"})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }

    // classVarDec
    mDepth--;
    std::cout << "</classVarDec>\n";
}

auto CompilationEngine::compileSubroutine() -> void
{
    // subroutineDec
    std::cout << "<subroutineDec>\n";
    mDepth++;

    // function void main (
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());

    // Write parameterlist, there can only be one.
    this->compileParameterList();

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());

    // call subroutinebody
    this->compileSubroutine();

    // subroutineDec
    mDepth--;
    std::cout << "</subroutineDec>\n";
}

auto CompilationEngine::compileSubroutine() -> void
{
    // subroutineBody
    std::cout << "<subroutineBody>\n";
    mDepth++;

    // {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());

    while (mTokenizer->getToken() != std::string{"}"})
    {
        mTokenizer->advance();
        if (mTokenizer->getToken() != std::string{"var"})
        {
            this->compileVarDecl();
        }
        else
        {
            this->compileStatements();
        }
    }

    // subroutineBody
    mDepth--;
    std::cout << "</parameterList>\n";
}

auto CompilationEngine::compileParameterList() -> void
{
    // parameterList
    std::cout << "<parameterList>\n";
    mDepth++;

    // int A, int B ...
    while (mTokenizer->getToken() != std::string{")"})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }

    // parameterList
    mDepth--;
    std::cout << "</parameterList>\n";
}

auto CompilationEngine::write(TokenType type, std::string data) -> void
{
    auto type_str = this->tokenTypeToString(type);

    for (int i = 0; i < mDepth; i++)
        std::cout << "  ";

    std::cout << "<";
    std::cout << type_str;
    std::cout << "> ";

    std::cout << data;

    std::cout << " </";
    std::cout << type_str;
    std::cout << ">\n";
};