#include <fstream>
#include <memory>

#include "JackTokenizer.h"

class CompilationEngine
{
private:
    std::unique_ptr<JackTokenizer> mJackTokenizer;
    std::ofstream mOutputFile;

public:
    CompilationEngine(std::unique_ptr<JackTokenizer> jackTokenizer, std::string outputFileName)
        : mJackTokenizer(std::move(jackTokenizer)), mOutputFile(outputFileName){};

    ~CompilationEngine();

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
};

CompilationEngine::~CompilationEngine()
{
}
