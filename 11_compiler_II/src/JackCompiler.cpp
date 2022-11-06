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

inline auto create_output_path(std::string in_path) -> std::string
{
    auto dot_pos = in_path.find(".");
    auto out_path = in_path.insert(dot_pos, std::string{"V"});
    return out_path;
}

int main(int argc, char *argv[])
{
    argc++;
    if (argc == 1)
    {
        throw std::invalid_argument("Please provide a path or filename.");
    }

    // std::string pathOrDir = std::string(argv[1]);
    auto pathOrDir = std::string{
        "/home/vincent/repos/nand2tetris/11_compiler_II/test/Average/Main.jack"};

    if (ends_with(pathOrDir, std::string{".jack"}))
    {
        // Single .jack file
        auto pathOut = create_output_path(pathOrDir);
        boost::replace_all(pathOut, ".jack", ".xml");

        // Create tokenizer to parse file
        auto tokenizer = std::make_unique<JackTokenizer>(pathOrDir);

        // Symbol table
        auto symboltable = std::make_unique<SymbolTable>();

        // Compilation Engine
        auto engine =
            CompilationEngine(std::move(symboltable), std::move(tokenizer), pathOut);

        // Writer
        // auto vmwriter = VMWriter(pathOut);

        // compile a file
        engine.compileClass();
    }
    // else
    // {

    //     for (const auto &dirEntry :
    //          std::filesystem::recursive_directory_iterator(pathOrDir))
    //     {
    //         std::cout << dirEntry << std::endl;

    //         // If not a jack file skip.
    //         if (!ends_with(dirEntry.path().string(), std::string{".jack"}))
    //             continue;

    //         // Input path
    //         auto pathIn = dirEntry.path();

    //         // Output path
    //         auto pathOut = create_output_path(pathIn.string());
    //         boost::replace_all(pathOut, ".jack", ".xml");

    //         // Engine
    //         auto engine = CompilationEngine(
    //             std::move(std::make_unique<JackTokenizer>(pathIn.string())), pathOut);

    //         // Compile.
    //         engine.compileClass();
    //     }
    // }
    return 0;
}