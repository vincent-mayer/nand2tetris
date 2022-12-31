#pragma once
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include "definitions.h"
#include <fstream>
#include <memory>

class CompilationEngine
{
private:
    std::unique_ptr<SymbolTable> mSymbolTable;
    std::unique_ptr<JackTokenizer> mTokenizer;
    std::unique_ptr<VMWriter> mVMWriter;
    std::ofstream mOutputFile;
    int mDepth;
    std::string mPrevType;
    std::string mClassName;
    int mLabelCounter;

public:
    CompilationEngine(std::unique_ptr<SymbolTable> symbolTable,
                      std::unique_ptr<JackTokenizer> jackTokenizer,
                      std::unique_ptr<VMWriter> vmWriter, std::string outputFileName)
        : mSymbolTable(std::move(symbolTable)), mTokenizer(std::move(jackTokenizer)),
          mVMWriter(std::move(vmWriter)), mOutputFile(outputFileName),
          mDepth(0), mPrevType{""}, mClassName{""}, mLabelCounter(0){};

    ~CompilationEngine()
    {
        // mOutputFile << "</tokens>\n";
        mOutputFile.close();
    };

    auto tokenTypeToString(TokenType token) -> std::string;

    auto handleIdentifier(std::string name) -> void;

    auto tokenizer() -> const std::unique_ptr<JackTokenizer> &
    {
        return mTokenizer;
    };

    auto compileClass() -> void;

    auto compileClassVarDecl() -> int;

    auto compileSubroutine(int nFields, FunctionType functionType) -> void;

    auto compileSubroutineBody(std::string name, int nFields, FunctionType functionType)
        -> void;

    auto compileParameterList() -> int;

    auto compileVarDecl() -> int;

    auto compileStatements() -> void;

    auto compileDo() -> void;

    auto compileLet() -> void;

    auto compileWhile() -> void;

    auto compileReturn() -> void;

    auto compileIf() -> void;

    auto compileExpression() -> int;

    auto compileTerm() -> void;

    auto compileExpressionList() -> int;

    auto write(TokenType tokenType, std::string data) -> void;

    auto write(std::string data) -> void;
};
