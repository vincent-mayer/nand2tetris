#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <boost/algorithm/string/replace.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>

inline auto ends_with(std::string const &value, std::string const ending) -> bool
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

inline auto create_output_path(std::string &in_path) -> std::string
{
    auto dot_pos = in_path.find(".");
    // auto out_path = in_path.insert(dot_pos, std::string{"V"});
    return in_path;
}

void compile(std::string &path)
{
    auto pathOut = create_output_path(path);
    auto pathOutXml = pathOut;
    auto pathOutVm = pathOut;
    boost::replace_all(pathOutXml, ".jack", ".xml");
    boost::replace_all(pathOutVm, ".jack", ".vm");

    // Create tokenizer to parse file
    auto tokenizer = std::make_unique<JackTokenizer>(path);

    // Writer
    auto vmwriter = std::make_unique<VMWriter>(pathOutVm);

    // Symbol table
    auto symboltable = std::make_unique<SymbolTable>();

    // Compilation Engine
    auto engine = CompilationEngine(std::move(symboltable), std::move(tokenizer),
                                    std::move(vmwriter), pathOutXml);

    // compile a file
    engine.compileClass();
}

int main(int argc, char *argv[])
{
    argc++;
    if (argc == 1)
    {
        throw std::invalid_argument("Please provide a path or filename.");
    }

    // std::string pathOrDir = std::string(argv[1]);
    auto pathOrDir =
        std::string{"/home/vincent/repos/nand2tetris/11_compiler_II/test/Square"};

    if (ends_with(pathOrDir, std::string{".jack"}))
    {
        compile(pathOrDir);
    }
    else
    {
        for (const auto &dirEntry :
             std::filesystem::recursive_directory_iterator(pathOrDir))
        {
            std::string path = dirEntry.path();
            if (path.ends_with(".jack"))
                compile(path);
        }
    }
    return 0;
}
