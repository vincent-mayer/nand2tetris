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
    mOutputFile << "<class>\n";
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
    mOutputFile << "</class>\n";
};

auto CompilationEngine::compileClassVarDecl() -> void
{
    // classVarDec
    mOutputFile << "<classVarDec>\n";
    mDepth++;

    while (mTokenizer->getToken() != std::string{";"})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }

    // classVarDec
    mDepth--;
    mOutputFile << "</classVarDec>\n";
}

auto CompilationEngine::compileSubroutine() -> void
{
    //
    // subroutineDec
    mOutputFile << "<subroutineDec>\n";
    mDepth++;

    // function void main
    while (mTokenizer->getToken() != std::string{"("})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }

    // Write parameterlist
    this->compileParameterList();

    while (mTokenizer->getToken() != std::string{"}"})
    {
    }

    // subroutineDec
    mDepth--;
    mOutputFile << "</subroutineDec>\n";
}

auto CompilationEngine::compileParameterList() -> void
{
    // (
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());

    // parameterList
    mOutputFile << "<parameterList>\n";
    mDepth++;

    // int A, int B ...
    while (mTokenizer->getToken() != std::string{")"})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->getToken());
    }

    // parameterList
    mDepth--;
    mOutputFile << "</parameterList>\n";

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->getToken());
}

auto CompilationEngine::write(TokenType type, std::string data) -> void
{
    auto type_str = this->tokenTypeToString(type);

    for (int i = 0; i < mDepth; i++)
        mOutputFile << "  ";

    mOutputFile << "<";
    mOutputFile << type_str;
    mOutputFile << "> ";

    mOutputFile << data;

    mOutputFile << " </";
    mOutputFile << type_str;
    mOutputFile << ">\n";
};