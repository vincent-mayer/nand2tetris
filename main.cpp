#include <bitset>
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
    // if (argc == 1 || argc > 2) {
    //     throw std::invalid_argument("Usage: 'assemble <input_file_name>.asm'");
    //     return 0;
    // }

    // // Create the needed objects
    // auto input_file_name  = std::string(argv[1]);
    auto input_file_name =
        std::string("/home/vincent/Documents/courses/nand2tetris/projects/06/assembler/input/Add.asm");
    auto parser           = Parser(input_file_name);
    auto translator       = CodeTranslator();
    auto stable           = SymbolTable();
    auto output_file_name = getOutputFileName(input_file_name);
    auto fout             = std::ofstream();

    // Create the output file
    fout.open(output_file_name);

    // First file pass to build table
    unsigned int rom_address = 0;
    while (parser.hasMoreCommands()) {
        parser.advance();
        auto command_type = parser.commandType();
        if (command_type == A || command_type == C)
            rom_address++;
        else if (command_type == L && !stable.contains(parser.symbol()))
            stable.addEntry(parser.symbol(), rom_address);
    }

    stable.printTable();

    // reset parser to beginning of file
    parser.reset();

    // Second file pass
    unsigned int var_ram_address = 16;
    int address                  = 0;

    while (parser.hasMoreCommands()) {
        parser.advance();
        auto command_type = parser.commandType();

        if (command_type == A) {
            auto symbol = parser.symbol();
            std::bitset<15> data(std::stoi(symbol));

            fout << "0";
            fout << data;
            fout << "\n";
            // if (stable.contains(symbol)) {
            //     address = stable.getAddress(symbol);
            // }
            // else {
            //     address = var_ram_address;
            //     stable.addEntry(symbol, var_ram_address);
            //     var_ram_address++;
            // }
        }
        else if (command_type == C) {
            auto comp = parser.comp();
            auto dest = parser.dest();
            auto jump = parser.jump();

            std::cout << "comp: " << comp << " ";
            std::cout << "dest: " << dest << " ";
            std::cout << "jump: " << jump << std::endl;

            fout << "111";
            fout << translator.comp(comp);
            fout << translator.dest(dest);
            fout << translator.jump(jump);
            fout << std::endl;
        }
        else {
            throw std::runtime_error("Invalid command type");
        }
    }
    fout.close();
    return 0;
}