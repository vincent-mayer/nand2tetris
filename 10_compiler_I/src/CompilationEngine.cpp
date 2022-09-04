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
        this->write(mTokenizer->tokenType(), mTokenizer->token());
    }
};

auto CompilationEngine::compileClass() -> void
{
    // class token
    this->write(std::string{"<class>"});
    mDepth++;

    // class Main {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    while (mTokenizer->token() != std::string{"}"})
    {
        // Get the next token.
        mTokenizer->advance();

        // If the token is function, compile subroutine.
        if (mTokenizer->token() == std::string{"function"} ||
            mTokenizer->token() == std::string{"method"})
            this->compileSubroutine();

        else if (mTokenizer->token() == std::string{"field"} ||
                 mTokenizer->token() == std::string{"static"})
            this->compileClassVarDecl();
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;

    // class
    this->write(std::string{"</class>"});
};

auto CompilationEngine::compileClassVarDecl() -> void
{
    // classVarDec
    this->write(std::string{"<classVarDec>"});
    mDepth++;

    while (mTokenizer->token() != std::string{";"})
    {
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->token());
    }

    // classVarDec
    mDepth--;
    this->write(std::string{"</classVarDec>"});
}

auto CompilationEngine::compileSubroutine() -> void
{
    // subroutineDec
    this->write(std::string{"<subroutineDec>"});
    mDepth++;

    // function void main (
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // Write parameterlist, there can only be one.
    this->compileParameterList();

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // call subroutinebody
    this->compileSubroutineBody();

    // subroutineDec
    mDepth--;
    this->write(std::string{"</subroutineDec>"});
}

auto CompilationEngine::compileSubroutineBody() -> void
{
    // subroutineBody
    this->write(std::string{"<subroutineBody>"});
    mDepth++;

    // {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    while (mTokenizer->token() != std::string{"}"})
    {
        mTokenizer->advance();
        if (mTokenizer->token() == std::string{"var"})
        {
            this->compileVarDecl();
        }
        else
        {
            this->compileStatements();
        }
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // subroutineBody
    mDepth--;
    this->write(std::string{"</subroutineBody>"});
}

auto CompilationEngine::compileVarDecl() -> void
{
    this->write(std::string{"<varDec>"});
    mDepth++;
    while (mTokenizer->token() != std::string{";"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write(std::string{"</varDec>"});
};

auto CompilationEngine::compileStatements() -> void
{
    this->write(std::string{"<statements>"});
    mDepth++;

    while (mTokenizer->token() != std::string{"}"})
    {
        if (mTokenizer->token() == std::string{"let"})
            this->compileLet();
        else if (mTokenizer->token() == std::string{"do"})
            this->compileDo();
        else if (mTokenizer->token() == std::string{"while"})
            this->compileWhile();
        else if (mTokenizer->token() == std::string{"return"})
            this->compileReturn();
        else if (mTokenizer->token() == std::string{"if"})
            this->compileIf();
        mTokenizer->advance();
    }

    mDepth--;
    this->write(std::string{"</statements>"});
}

auto CompilationEngine::compileLet() -> void
{
    this->write(std::string{"<letStatement>"});
    mDepth++;

    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    while (mTokenizer->token() != std::string{";"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());

        if (mTokenizer->token() == std::string{"="} ||
            mTokenizer->token() == std::string{"["})
        {
            compileExpression();
        }
        else
        {
            mTokenizer->advance();
        }
    }
    // ;
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write(std::string{"</letStatement>"});
}

auto CompilationEngine::compileExpression() -> void
{
    mTokenizer->advance();
    if (mTokenizer->token() == std::string{")"})
        return;
    this->write(std::string{"<expression>"});
    mDepth++;

    while (mTokenizer->token() != std::string{";"} &&
           mTokenizer->token() != std::string{"]"} &&
           mTokenizer->token() != std::string{")"})
    {
        this->compileTerm();
        if (std::find(ops.begin(), ops.end(), mTokenizer->token()) != ops.end())
        {
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
        }
    }
    mDepth--;
    this->write(std::string{"</expression>"});
}

auto CompilationEngine::compileTerm() -> void
{
    this->write(std::string{"<term>"});
    mDepth++;
    while (mTokenizer->token() != std::string{")"} &&
           mTokenizer->token() != std::string{";"} &&
           mTokenizer->token() != std::string{"]"} &&
           std::find(ops.begin(), ops.end(), mTokenizer->token()) == ops.end())
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        if (mTokenizer->token() == std::string{"("})
        {
            this->compileExpressionList();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
        }
        if (mTokenizer->token() == std::string{"["})
        {
            this->compileExpression();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
            break;
        }

        // )
        mTokenizer->advance();
    }
    mDepth--;
    this->write(std::string{"</term>"});
}

auto CompilationEngine::compileExpressionList() -> void
{
    this->write(std::string{"<expressionList>"});
    mDepth++;
    while (mTokenizer->token() != std::string{")"})
        this->compileExpression();
    mDepth--;
    this->write(std::string{"</expressionList>"});
}
auto CompilationEngine::compileDo() -> void
{
    this->write(std::string{"<doStatement>"});
    mDepth++;

    while (mTokenizer->token() != std::string{";"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        if (mTokenizer->token() == std::string{"("})
        {
            this->compileExpressionList();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
        }
        mTokenizer->advance();
    }

    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write(std::string{"</doStatement>"});
};
auto CompilationEngine::compileWhile() -> void
{
    this->write(std::string{"<whileStatement>"});
    mDepth++;
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    while (mTokenizer->token() != std::string{"}"})
    {

        if (mTokenizer->token() == std::string{"("})
        {
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            this->compileExpression();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == std::string{"{"})
        {
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
            this->compileStatements();
        }
    }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write(std::string{"</whileStatement>"});
};
auto CompilationEngine::compileReturn() -> void
{
    // returnStatement
    this->write(std::string{"<returnStatement>"});
    mDepth++;

    // return
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    while (mTokenizer->token() != std::string{";"})
    {
        this->compileExpression();
    }

    // ;
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // returnStatement
    mDepth--;
    this->write(std::string{"</returnStatement>"});
};
auto CompilationEngine::compileIf() -> void
{
    // ifStatement
    this->write(std::string{"<ifStatement>"});
    mDepth++;

    // return
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // ifStatement
    mDepth--;
    this->write(std::string{"</ifStatement>"});
};

auto CompilationEngine::compileParameterList() -> void
{
    // parameterList
    this->write(std::string{"<parameterList>"});
    mDepth++;

    // Advance once
    mTokenizer->advance();

    // int A, int B ...
    while (mTokenizer->token() != std::string{")"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }

    // parameterList
    mDepth--;
    this->write(std::string{"</parameterList>"});
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

auto CompilationEngine::write(std::string data) -> void
{
    for (int i = 0; i < mDepth; i++)
        mOutputFile << "  ";
    mOutputFile << data;
    mOutputFile << "\n";
}