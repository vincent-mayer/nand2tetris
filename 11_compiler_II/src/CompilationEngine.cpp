#include "CompilationEngine.h"
#include "definitions.h"
#include <iostream>
#include <magic_enum.hpp>
#include <string>

auto CompilationEngine::tokenTypeToString(TokenType token) -> std::string
{
    std::string type = "";
    if (mTokenizer->tokenType() == TokenType::KEYWORD)
        type = "keyword";
    else if (mTokenizer->tokenType() == TokenType::SYMBOL)
        type = "symbol";
    else if (mTokenizer->tokenType() == TokenType::IDENTIFIER)
        type = "identifier";
    else if (mTokenizer->tokenType() == TokenType::INT_CONST)
        type = "integerConstant";
    else if (mTokenizer->tokenType() == TokenType::STRING_CONST)
        type = "stringConstant";

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
    if (type == "var")
        return;

    if (type == ",")
        type = mPrevType;

    // Insert into table.
    if (kind != Kind::NONE && std::all_of(name.begin(), name.end(), &::islower) &&
        type != ".")
        mSymbolTable->define(name, type, kind);
    else if (kind == Kind::ARG)
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
    this->write("<class>");
    mDepth++;

    // class Main {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mClassName = mTokenizer->token();
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    int nFields = 0;
    while (mTokenizer->token() != "}")
    {
        if (mTokenizer->token() == "field" || mTokenizer->token() == "static")
            nFields += this->compileClassVarDecl();
        else if (mTokenizer->token() == "constructor")
            this->compileSubroutine(nFields, FunctionType::CONSTRUCTOR);
        else if (mTokenizer->token() == "function")
            this->compileSubroutine(nFields, FunctionType::FUNCTION);
        else if (mTokenizer->token() == "method")
            this->compileSubroutine(nFields, FunctionType::METHOD);

        // Get the next token.
        mTokenizer->advance();
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;

    // class
    this->write("</class>");
};

auto CompilationEngine::compileClassVarDecl() -> int
{
    // classVarDec
    this->write("<classVarDec>");
    mDepth++;

    int nFields = 0;
    while (mTokenizer->token() != ";")
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        // Need to know number of attributes for the class.
        if (mSymbolTable->indexOf(mTokenizer->token()) > -1)
            nFields++;
        mTokenizer->advance();
    }

    //;
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // classVarDec
    mDepth--;
    this->write("</classVarDec>");

    return nFields;
}

auto CompilationEngine::compileSubroutine(int nFields, FunctionType functionType) -> void
{
    // subroutineDec
    this->write("<subroutineDec>");
    mDepth++;
    mSymbolTable->startSubroutine();

    // function void main (
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    mTokenizer->setKind(Kind::NONE); // Hacky way to prevent constr. entry in symboltable
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    std::string funcName = mTokenizer->token();
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // Write parameterlist, there can only be one.
    this->compileParameterList();

    // )
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // call subroutinebody
    this->compileSubroutineBody(funcName, nFields, functionType);

    // subroutineDec
    mDepth--;
    this->write("</subroutineDec>");
}

auto CompilationEngine::compileSubroutineBody(std::string name, int nFields,
                                              FunctionType functionType) -> void
{
    // subroutineBody
    this->write("<subroutineBody>");
    mDepth++;

    // {
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    int nLocals = 0;
    while (mTokenizer->token() != "}")
    {
        mTokenizer->advance();
        if (mTokenizer->token() == "var")
        {
            nLocals += this->compileVarDecl();
        }
        else
        {
            // function main.Main {nLocals}
            mVMWriter->writeFunction(mClassName, name, nLocals);
            // Special case constructor.
            if (functionType == FunctionType::CONSTRUCTOR)
            {
                // Allocate memory for the class and set pointer.
                mVMWriter->writePush(Segment::CONST, nFields);
                mVMWriter->writeCall("Memory.alloc", 1);
                mVMWriter->writePop(Segment::POINTER, 0);
            }
            // First argument is implicit this.
            else if (functionType == FunctionType::METHOD)
            {
                mVMWriter->writePush(Segment::ARG, 0);
                mVMWriter->writePop(Segment::POINTER, 0);
            }
            // Else just do nothing at all
            else
            {
            }
            this->compileStatements();
        }
    }

    // }
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // subroutineBody
    mDepth--;
    this->write("</subroutineBody>");
}

auto CompilationEngine::compileVarDecl() -> int
{
    this->write("<varDec>");
    mDepth++;
    int mVars = 0;
    while (mTokenizer->token() != ";")
    {
        if ((mTokenizer->tokenType() == TokenType::IDENTIFIER) &&
            (std::islower(mTokenizer->token()[0])))
            mVars++;
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
    }
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write("</varDec>");
    return mVars;
};

auto CompilationEngine::compileStatements() -> void
{
    this->write("<statements>");
    mDepth++;

    while (mTokenizer->token() != "}")
    {
        if (mTokenizer->token() == "let")
        {
            this->compileLet();
        }
        else if (mTokenizer->token() == "do")
        {
            this->compileDo();
        }
        else if (mTokenizer->token() == "while")
        {
            this->compileWhile();
        }
        else if (mTokenizer->token() == "return")
        {
            this->compileReturn();
        }
        else if (mTokenizer->token() == "if")
        {
            auto ifonly = this->compileIf();
            if (ifonly)
                continue;
        }
        mTokenizer->advance();
    }

    mDepth--;
    this->write("</statements>");
}

auto CompilationEngine::compileLet() -> void
{
    this->write("<letStatement>");
    mDepth++;

    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    std::string varName;
    bool isArr = false;
    while (mTokenizer->token() != ";")
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());

        if (mTokenizer->token() == "=")
        {
            varName = mTokenizer->prevToken();
            compileExpression();
        }
        else if (mTokenizer->token() == "[")
        {
            isArr = true;
            varName = mTokenizer->prevToken();
            auto kind = mSymbolTable->kindOf(varName);
            auto varIndex = mSymbolTable->indexOf(varName);
            mVMWriter->writePush(kindToSegment[kind], varIndex);
            compileExpression();
        }
        else
        {
            mTokenizer->advance();
        }
    }
    // ;
    if (isArr)
    {
        // auto varCount = mSymbolTable->varCount()
        mVMWriter->writePop(Segment::TEMP, 1);
        mVMWriter->writeArithmetic("+");
        mVMWriter->writePop(Segment::POINTER, 1);
        mVMWriter->writePush(Segment::TEMP, 1);
        mVMWriter->writePop(Segment::THAT, 0);
    }
    else
    {
        auto kind = mSymbolTable->kindOf(varName);
        auto varIndex = mSymbolTable->indexOf(varName);
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mVMWriter->writePop(kindToSegment[kind], varIndex);
    }
    mDepth--;
    this->write("</letStatement>");
}

auto CompilationEngine::compileExpression() -> int
{
    mTokenizer->advance();
    if (mTokenizer->token() == ")" || mTokenizer->token() == ";")
        return 0;
    this->write("<expression>");
    mDepth++;

    std::string curOperator = "";
    while (mTokenizer->token() != ";" && mTokenizer->token() != "]" &&
           mTokenizer->token() != ")")
    {
        this->compileTerm();
        if (mTokenizer->token() == ",")
            break;
        if (std::find(ops.begin(), ops.end(), mTokenizer->token()) != ops.end())
        {
            curOperator = mTokenizer->token();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
        }
    }
    if (curOperator != "")
        mVMWriter->writeArithmetic(curOperator);
    mDepth--;
    this->write("</expression>");
    return 1;
}

auto CompilationEngine::compileString() -> void
{
    auto token = mTokenizer->token();

    // Push character length on stack
    mVMWriter->writePush(Segment::CONST, token.length());
    mVMWriter->writeCall("String.new", 1);
    for (const char &c : token)
    {
        int cascii = static_cast<int>(c);
        mVMWriter->writePush(Segment::CONST, cascii);
        mVMWriter->writeCall("String.appendChar", 2);
    }
}

auto CompilationEngine::compileTerm() -> void
{
    this->write("<term>");
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
        if (mTokenizer->token() == "true" || mTokenizer->token() == "false")
        {
            mVMWriter->writePush(Segment::CONST, 0);
            if (mTokenizer->token() == "true")
                mVMWriter->writeArithmetic("~");
        }
        if (mTokenizer->token() == "this")
        {
            mVMWriter->writePush(Segment::POINTER, 0);
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
                mVMWriter->writeCall(className + "." + funcName, nArgs);
                isFuncCall = false;
            }
        }
        if (mTokenizer->token() == "[")
        {
            this->compileExpression();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mVMWriter->writeArithmetic("+");
            mVMWriter->writePop(Segment::POINTER, 1);
            mVMWriter->writePush(Segment::THAT, 0);
            mTokenizer->advance();
            break;
        }
        if (std::find(ops.begin(), ops.end(), mTokenizer->token()) != ops.end())
        {
            isOp = true;
            if (mTokenizer->prevTokenType() == TokenType::SYMBOL &&
                mTokenizer->token() != "~")
                isNegNotOp = true;
            else
                isNegNotOp = false;
        }
        if (mTokenizer->token() == ".")
        {
            isFuncCall = true;
            auto token = mTokenizer->prevToken();
            // Class names must start with caps.
            if (std::isupper(token[0]))
                className = token;
            else
                className = mSymbolTable->typeOf(token);
        }
        // Special case compilation for strings.
        if (mTokenizer->tokenType() == TokenType::STRING_CONST)
            this->compileString();
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
    this->write("</term>");
}

auto CompilationEngine::compileExpressionList() -> int
{
    this->write("<expressionList>");
    mDepth++;
    int nArgs = 0;
    while (mTokenizer->token() != ")")
    {
        nArgs += this->compileExpression();
        if (mTokenizer->token() == ",")
            this->write(mTokenizer->tokenType(), mTokenizer->token());
    }
    mDepth--;
    this->write("</expressionList>");
    return nArgs;
}
auto CompilationEngine::compileDo() -> void
{
    this->write("<doStatement>");
    mDepth++;

    std::string callClass;
    std::string callMethod;
    int nArgs = 0;
    bool callClassIsVar = false;
    while (mTokenizer->token() != ";")
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        if (mTokenizer->token() == "(")
        {
            callMethod = mTokenizer->prevToken();
            nArgs = this->compileExpressionList();
            this->write(mTokenizer->tokenType(), mTokenizer->token());
        }
        mTokenizer->advance();

        // Remember the class name for function call
        if (mTokenizer->prevToken() == "do")
        {
            // Class names must start with caps.
            std::string token = mTokenizer->token();
            if (std::isupper(token[0]))
                callClass = token;
            else
            {
                // TODO: Note sure this is correct.
                callClass = mSymbolTable->typeOf(token);
                auto kind = mSymbolTable->kindOf(token);
                auto index = mSymbolTable->indexOf(token);

                // Handle the special case where the called method belongs to this.
                if (index == -1)
                {
                    callClass = mClassName;
                    mVMWriter->writePush(Segment::POINTER, 0);
                }
                else
                {
                    mVMWriter->writePush(kindToSegment[kind], index);
                }
            }
        }
    }

    // We increment the function call by 1 because of the implicit this pointer.
    // However for OS/built-on functions this should not happen.
    if ((mSymbolTable->knownType(callClass) || (mClassName == callClass)) &&
        callClass != "Main")
        nArgs++;

    // Write the function call.
    mVMWriter->writeCall(callClass + "." + callMethod, nArgs);

    // Pop the implicit returned 0
    // TODO: Only for void functions!
    mVMWriter->writePop(Segment::TEMP, 0);

    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write("</doStatement>");
};
auto CompilationEngine::compileWhile() -> void
{
    this->write("<whileStatement>");
    mDepth++;
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    // Label L1
    mVMWriter->writeLabel("WHILE_START" + std::to_string(mLabelCounter));
    int localLabelCounter = mLabelCounter;
    mLabelCounter++;
    while (mTokenizer->token() != "}")
    {

        if (mTokenizer->token() == "(")
        {
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            // cond
            this->compileExpression();
            // Write explicit not, because expression must be not'ed in while.
            mVMWriter->writeArithmetic("not");
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            // if-goto L2
            mVMWriter->writeIf("WHILE_END" + std::to_string(localLabelCounter));
            mTokenizer->advance();
        }
        else if (mTokenizer->token() == "{")
        {
            this->write(mTokenizer->tokenType(), mTokenizer->token());
            mTokenizer->advance();
            // s1
            this->compileStatements();
        }
    }
    // goto L1
    mVMWriter->writeGoto("WHILE_START" + std::to_string(localLabelCounter));

    // label L2
    mVMWriter->writeLabel("WHILE_END" + std::to_string(localLabelCounter));

    // rest
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mDepth--;
    this->write("</whileStatement>");
};
auto CompilationEngine::compileReturn() -> void
{
    // returnStatement
    this->write("<returnStatement>");
    mDepth++;

    // return
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    int terms;
    while (mTokenizer->token() != ";")
    {
        terms = this->compileExpression();
    }

    // Push return 0 constant
    if (terms == 0)
        mVMWriter->writePush(Segment::CONST, 0);
    mVMWriter->writeReturn();

    // ;
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // returnStatement
    mDepth--;
    this->write("</returnStatement>");
};
auto CompilationEngine::compileIf() -> bool
{
    int localLabelCounter = mLabelCounter;
    mLabelCounter++;
    // ifStatement
    this->write("<ifStatement>");
    mDepth++;

    // if
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    // (
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // cond
    this->compileExpression();

    // ~(cond)
    mVMWriter->writeArithmetic("not"); // ~(cond)

    // if-goto L1: L1 = IF_FALSE; L2 = IF_END
    mVMWriter->writeIf("IF_FALSE" + std::to_string(localLabelCounter));

    // ) {
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();
    this->write(mTokenizer->tokenType(), mTokenizer->token());

    // s1; }
    this->compileStatements();
    this->write(mTokenizer->tokenType(), mTokenizer->token());
    mTokenizer->advance();

    // goto label L2 = IF_END
    mVMWriter->writeGoto("IF_END" + std::to_string(localLabelCounter));

    // label L1
    mVMWriter->writeLabel("IF_FALSE" + std::to_string(localLabelCounter));

    // else
    bool ifOnly = true;
    if (mTokenizer->token() == "else")
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        this->compileStatements();
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        ifOnly = false;
    }

    // label L2 = IF_END
    mVMWriter->writeLabel("IF_END" + std::to_string(localLabelCounter));

    // ifStatement
    mDepth--;
    this->write("</ifStatement>");
    return ifOnly;
};

auto CompilationEngine::compileParameterList() -> int
{
    // parameterList
    this->write("<parameterList>");
    mDepth++;

    // Advance once
    mTokenizer->advance();

    // int A, int B ...
    mTokenizer->setKind(Kind::ARG);
    int nParameters = 0;
    while (mTokenizer->token() != ")")
    {
        this->write(mTokenizer->tokenType(), mTokenizer->token());
        mTokenizer->advance();
        nParameters++;
    }

    // parameterList
    mDepth--;
    this->write("</parameterList>");
    return nParameters;
}