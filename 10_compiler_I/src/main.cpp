#include <CompilationEngine.h>
#include <JackTokenizer.h>
#include <chrono>

int main(int argc, char *argv[])
{
    std::string path_in = "/home/vincent/repos/nand2tetris/10_compiler_I/test/"
                          "Square/Main.jack";
    std::string path_out = "/home/vincent/repos/nand2tetris/10_compiler_I/test/"
                           "Square/MainV.xml";

    std::unique_ptr<JackTokenizer> tokenizer = std::make_unique<JackTokenizer>(path_in);

    CompilationEngine engine(std::move(std::make_unique<JackTokenizer>(path_in)),
                             path_out);

    engine.compileClass();

    return 0;
}