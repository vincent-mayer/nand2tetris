#include "CompilationEngine.h"
#include "definitions.h"
#include <magic_enum.hpp>
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

auto CompilationEngine::write(TokenType type, std::string data) -> void
{
    auto type_str = this->tokenTypeToString(type);

    for (int i = 0; i < mDepth; i++)
        mOutputFile << "  ";

    mOutputFile << "<";
    mOutputFile << type_str;
    mOutputFile << "> ";

    mOutputFile << data;

    // Special case handling for identifier.
    if (type == TokenType::IDENTIFIER)
        this->handleIdentifier(data);

    mOutputFile << " </";
    mOutputFile << type_str;
    mOutputFile << ">\n";
};

auto CompilationEngine::handleIdentifier(std::string name) -> void
{
    // Determine kind.
    auto kind = mTokenizer->lastKind();

    // Determine type.
    auto type = mTokenizer->prevToken();

    // Edge-casing, to avoid including int, Array etc. Needs static and field and stuff
    // too.
    if (type == std::string{"var"})
        return;

    if (type == std::string{","})
        type = mPrevType;

    // Insert into table.
    if (kind != Kind::NONE && std::all_of(name.begin(), name.end(), &::islower) &&
        type != ".")
        mSymbolTable->define(name, type, kind);

    // Get the correct index.
    auto index = mSymbolTable->indexOf(name);

    // Write all data.
    mOutputFile << ";type:" << type << ";kind:";
    mOutputFile << magic_enum::enum_name(kind) << ";index:";
    mOutputFile << std::to_string(index) << ";";

    // Remember type
    mPrevType = type;
};

auto CompilationEngine::write(std::string data) -> void
{
    for (int i = 0; i < mDepth; i++)
        mOutputFile << "  ";
    mOutputFile << data;
    mOutputFile << "\n";
}

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
    mClassName = mTokenizer->token();
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    while (mTokenizer->token() != std::string{"}"})
    {

        // If the token is function, compile subroutine.
        if (mTokenizer->token() == std::string{"function"} ||
            mTokenizer->token() == std::string{"method"} ||
            mTokenizer->token() == std::string{"constructor"})
            this->compileSubroutine();

        else if (mTokenizer->token() == std::string{"field"} ||
                 mTokenizer->token() == std::string{"static"})
            this->compileClassVarDecl();

        // Get the next token.
        mTokenizer->advance();
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
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }

    //;
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // classVarDec
    mDepth--;
    this->write(std::string{"</classVarDec>"});
}

auto CompilationEngine::compileSubroutine() -> void
{
    // subroutineDec
    this->write(std::string{"<subroutineDec>"});
    mDepth++;
    mSymbolTable->startSubroutine();

    // function void main (
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    std::string funcName = mTokenizer->token();
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // Write parameterlist, there can only be one.
    this->compileParameterList();

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // call subroutinebody
    this->compileSubroutineBody(funcName);

    // subroutineDec
    mDepth--;
    this->write(std::string{"</subroutineDec>"});
}

auto CompilationEngine::compileSubroutineBody(std::string name) -> void
{
    // subroutineBody
    this->write(std::string{"<subroutineBody>"});
    mDepth++;

    // {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    int nLocals = 0;
    while (mTokenizer->token() != std::string{"}"})
    {
        mTokenizer->advance();
        if (mTokenizer->token() == std::string{"var"})
        {
            nLocals += this->compileVarDecl();
        }
        else
        {
            // function main.Main {nLocals}
            mVMWriter->writeFunction(mClassName, name, nLocals);
            this->compileStatements();
        }
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // subroutineBody
    mDepth--;
    this->write(std::string{"</subroutineBody>"});
}

auto CompilationEngine::compileVarDecl() -> int
{
    this->write(std::string{"<varDec>"});
    mDepth++;
    int mVars = 0;
    while (mTokenizer->token() != std::string{";"})
    {
        if (mTokenizer->tokenType() == TokenType::IDENTIFIER)
            mVars++;
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write(std::string{"</varDec>"});
    return mVars;
};

auto CompilationEngine::compileStatements() -> void
{
    this->write(std::string{"<statements>"});
    mDepth++;

    while (mTokenizer->token() != std::string{"}"})
    {
        if (mTokenizer->token() == std::string{"let"})
        {
            this->compileLet();
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == std::string{"do"})
        {
            this->compileDo();
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == std::string{"while"})
        {
            this->compileWhile();
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == std::string{"return"})
        {
            this->compileReturn();
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == std::string{"if"})
        {
            this->compileIf();
        }
        else
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

    std::string varName;
    while (mTokenizer->token() != std::string{";"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());

        if (mTokenizer->token() == std::string{"="} ||
            mTokenizer->token() == std::string{"["})
        {
            varName = mTokenizer->prevToken();
            compileExpression();
        }
        else
        {
            mTokenizer->advance();
        }
    }
    // ;
    auto kind = mSymbolTable->kindOf(varName);
    auto varIndex = mSymbolTable->indexOf(varName);
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mVMWriter->writePop(Segment::LOCAL, varIndex);
    mDepth--;
    this->write(std::string{"</letStatement>"});
}

auto CompilationEngine::compileExpression() -> int
{
    mTokenizer->advance();
    if (mTokenizer->token() == std::string{")"} ||
        mTokenizer->token() == std::string{";"})
        return 0;
    this->write(std::string{"<expression>"});
    mDepth++;

    std::string curOperator = "";
    while (mTokenizer->token() != std::string{";"} &&
           mTokenizer->token() != std::string{"]"} &&
           mTokenizer->token() != std::string{")"})
    {
        this->compileTerm();
        if (mTokenizer->token() == std::string{","})
            break;
        if (std::find(ops.begin(), ops.end(), mTokenizer->token()) != ops.end())
        {
            curOperator = mTokenizer->token();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
        }
    }
    if (curOperator != std::string{""})
        mVMWriter->writeArithmetic(curOperator);
    mDepth--;
    this->write(std::string{"</expression>"});
    return 1;
}

auto CompilationEngine::compileTerm() -> void
{
    this->write(std::string{"<term>"});
    mDepth++;
    int i = 0;
    int nArgs = 0;
    bool isOp = false;
    bool isNegNotOp = false;
    bool isFuncCall = false;
    std::string className;
    std::string funcName;
    std::string opName = "";
    while (mTokenizer->token() != ")" && mTokenizer->token() != ";" &&
           mTokenizer->token() != "]" &&
           (std::find(ops.begin(), ops.end(), mTokenizer->token()) == ops.end() ||
            (i == 0)))
    {
        // Push integer constants on stack.
        if (mTokenizer->tokenType() == TokenType::INT_CONST)
            mVMWriter->writePush(Segment::CONST, std::stoi(mTokenizer->token()));
        if (mTokenizer->tokenType() == TokenType::IDENTIFIER &&
            mSymbolTable->indexOf(mTokenizer->token()) != -1)
        {
            auto index = mSymbolTable->indexOf(mTokenizer->token());
            auto kind = mSymbolTable->kindOf(mTokenizer->token());
            mVMWriter->writePush(kindToSegment[kind], index);
        }
        if (mTokenizer->token() == "true")
        {
            mVMWriter->writePush(Segment::CONST, 0);
            mVMWriter->writeArithmetic("~");
        }

        this->write(mTokenizer->tokenType(), mTokenizer->token());
        if (mTokenizer->token() == "(")
        {
            funcName = mTokenizer->prevToken();
            if (mTokenizer->prevTokenType() == TokenType::IDENTIFIER)
                nArgs = this->compileExpressionList();
            else
                this->compileExpression();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            if (isFuncCall)
            {
                mVMWriter->writeCall(className + std::string{"."} + funcName, nArgs);
                isFuncCall = false;
            }
        }
        if (mTokenizer->token() == "[")
        {
            this->compileExpression();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
            break;
        }
        if (std::find(ops.begin(), ops.end(), mTokenizer->token()) != ops.end())
        {
            isOp = true;
            isNegNotOp =
                (mTokenizer->prevTokenType() == TokenType::SYMBOL) ? true : false;
        }
        if (mTokenizer->token() == ".")
        {
            isFuncCall = true;
            className = mTokenizer->prevToken();
        }
        // )
        auto opName = mTokenizer->token();
        mTokenizer->advance();

        if (mTokenizer->tokenType() == TokenType::INT_CONST ||
            mTokenizer->tokenType() == TokenType::STRING_CONST ||
            (mTokenizer->tokenType() == TokenType::IDENTIFIER && (i == 0)) ||
            mTokenizer->token() == "(" &&
                std::find(ops.begin(), ops.end(), mTokenizer->prevToken()) != ops.end())
        {
            this->compileTerm();
            if (isOp)
            {
                if (isNegNotOp)
                    mVMWriter->writeArithmetic(commandToString["neg"]);
                else
                    mVMWriter->writeArithmetic(commandToString[opName]);
                isOp = false;
            }
        }

        i++;
    }
    mDepth--;
    this->write(std::string{"</term>"});
}

auto CompilationEngine::compileExpressionList() -> int
{
    this->write(std::string{"<expressionList>"});
    mDepth++;
    int nArgs = 0;
    while (mTokenizer->token() != std::string{")"})
    {
        this->compileExpression();
        if (mTokenizer->token() == std::string{","})
            this->write(mTokenizer->tokenType(), mTokenizer->token());
        nArgs++;
    }
    mDepth--;
    this->write(std::string{"</expressionList>"});
    return nArgs;
}
auto CompilationEngine::compileDo() -> void
{
    this->write(std::string{"<doStatement>"});
    mDepth++;

    std::string callClass;
    std::string callMethod;
    int nArgs = 0;
    while (mTokenizer->token() != std::string{";"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        if (mTokenizer->token() == std::string{"("})
        {
            callMethod = mTokenizer->prevToken();
            nArgs = this->compileExpressionList();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
        }
        mTokenizer->advance();

        // Remember the class name for function call
        if (mTokenizer->prevToken() == std::string{"do"})
            callClass = mTokenizer->token();
    }

    // Write the function call.
    mVMWriter->writeCall(callClass + std::string{"."} + callMethod, nArgs);

    // Pop the implicit returned 0
    // TODO: Only for void functions!
    mVMWriter->writePop(Segment::TEMP, 0);

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
    mVMWriter->writeLabel("while0");
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

    int terms;
    while (mTokenizer->token() != std::string{";"})
    {
        terms = this->compileExpression();
    }

    // Push return 0 constant
    if (terms == 0)
    {
        mVMWriter->writePush(Segment::CONST, 0);
        mVMWriter->writeReturn();
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

    // if
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    // (
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // expression
    this->compileExpression();

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // statements
    this->compileStatements();

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    mTokenizer->advance();

    // else
    if (mTokenizer->token() == std::string{"else"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        this->compileStatements();
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }

    // ifStatement
    mDepth--;
    this->write(std::string{"</ifStatement>"});
};

auto CompilationEngine::compileParameterList() -> int
{
    // parameterList
    this->write(std::string{"<parameterList>"});
    mDepth++;

    // Advance once
    mTokenizer->advance();

    // int A, int B ...
    int nParameters = 0;
    while (mTokenizer->token() != std::string{")"})
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
        nParameters++;
    }

    // parameterList
    mDepth--;
    this->write(std::string{"</parameterList>"});
    return nParameters;
}