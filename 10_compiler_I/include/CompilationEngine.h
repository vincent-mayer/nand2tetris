#pragma once
#include "JackTokenizer.h"
#include "definitions.h"
#include <fstream>
#include <memory>

class CompilationEngine
{
private:
    std::unique_ptr<JackTokenizer> mTokenizer;
    std::ofstream mOutputFile;
    int mDepth;

public:
    CompilationEngine(std::unique_ptr<JackTokenizer> jackTokenizer,
                      std::string outputFileName)
        : mTokenizer(std::move(jackTokenizer)), mOutputFile(outputFileName),
          mDepth(0){
              // mOutputFile << "<tokens>\n";
          };

    ~CompilationEngine()
    {
        // mOutputFile << "</tokens>\n";
        mOutputFile.close();
    };

    auto tokenTypeToString(TokenType token) -> std::string;

    auto getTokenizer() -> const std::unique_ptr<JackTokenizer> &
    {
        return mTokenizer;
    };

    auto easyCompile() -> void;

    auto compileClass() -> void;

    auto compileClassVarDecl() -> void;

    auto compileSubroutine() -> void;

    auto compileParameterList() -> void;

    auto compileVarDecl() -> void;

    auto compileStatements() -> void;

    auto compileDo() -> void;

    auto compileLet() -> void;

    auto compileWhile() -> void;

    auto compileReturn() -> void;

    auto compileIf() -> void;

    auto compileExpression() -> void;

    auto compilTerm() -> void;

    auto compileExpressionList() -> void;

    auto write(TokenType tokenType, std::string data) -> void;
};
