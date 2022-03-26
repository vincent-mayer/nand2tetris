#include <fstream>
#include <iostream>
#include "CodeTranslator.h"
#include "Parser.h"
#include "SymbolTable.h"

/*
1. Assembler that trranslates programs without symbols
2. Ability to handle symbols
3. Merge 1. and 2.
*/

auto getOutputFileName(std::string& input_file) -> std::string
{
    unsigned long last_slash_pos    = input_file.rfind("/");
    std::string file_name           = input_file.substr(last_slash_pos + 1, input_file.size() - last_slash_pos - 1);
    unsigned long last_dot_pos      = file_name.rfind(".");
    std::string file_name_no_ending = file_name.substr(0, last_dot_pos);
    return std::string{"output/"} + file_name_no_ending + std::string{".hack"};
}

int main(int argc, char* argv[])
{
    if (argc == 1 || argc > 2) {
        throw std::invalid_argument("Usage: 'assemble <input_file_name>.asm'");
        return 0;
    }

    // Create the needed objects
    auto input_file_name  = std::string(argv[1]);
    auto parser           = Parser(input_file_name);
    auto translator       = CodeTranslator();
    auto table            = SymbolTable();
    auto output_file_name = getOutputFileName(input_file_name);
    auto output_file      = std::ofstream();

    // Create the output file
    output_file.open(output_file_name);

    // First file pass to build table
    // while (parser.hasMoreCommands()) {
    //     parser.advance();
    // }

    // reset parser to beginning of file
    parser.reset();

    // Second file pass
    while (parser.hasMoreCommands()) {
        parser.advance();
        auto command_type = parser.commandType();

        if (command_type == A) {
            auto symbol = parser.symbol();
        }
        else if (command_type == L) {
            auto symbol = parser.symbol();
        }
        else if (command_type == C) {
            auto comp = parser.comp();
            auto dest = parser.dest();
            auto jump = parser.jump();

            std::cout << "comp: " << comp << " ";
            std::cout << "dest: " << dest << " ";
            std::cout << "jump: " << jump << std::endl;
        }
        else
            throw std::runtime_error("Invalid command type");
    }
    return 0;
}