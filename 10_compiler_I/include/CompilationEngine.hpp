#include <JackTokenizer.hpp>
#include <fstream>
#include <memory>

class CompilationEngine
{
private:
    std::unique_ptr<JackTokenizer> mJackTokenizer;
    std::ofstream mOutputFile;

public:
    CompilationEngine(std::unique_ptr<JackTokenizer> jackTokenizer, std::string output_file_name) : mJackTokenizer(std::move(jackTokenizer)), {};
    ~CompilationEngine();
};

CompilationEngine::~CompilationEngine()
{
}
