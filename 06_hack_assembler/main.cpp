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
    if (argc == 1 || argc > 2) {
        throw std::invalid_argument("Usage: 'assemble <input_file_name>.asm'");
        return 0;
    }

    // Create the needed objects
    auto input_file_name  = std::string(argv[1]);
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

    // reset parser to beginning of file
    parser.reset();

    // Second file pass
    unsigned int var_ram_address = 16;
    int address                  = 0;

    while (parser.hasMoreCommands()) {
        parser.advance();
        if (!parser.hasMoreCommands()) {
            break;
        }

        auto command_type = parser.commandType();

        if (command_type == A) {
            auto symbol        = parser.symbol();
            unsigned int value = 0;
            if (parser.startsWithDigit(symbol)) {
                value = std::stoi(symbol);
            }
            else {
                if (stable.contains(symbol)) {
                    value = stable.getAddress(symbol);
                }
                else {
                    value = var_ram_address;
                    stable.addEntry(symbol, var_ram_address);
                    var_ram_address++;
                }
            }

            std::bitset<15> data(value);
            fout << "0";
            fout << data;
            fout << "\n";
        }
        else if (command_type == C) {
            auto comp = parser.comp();
            auto dest = parser.dest();
            auto jump = parser.jump();

            fout << "111";
            fout << translator.comp(comp);
            fout << translator.dest(dest);
            fout << translator.jump(jump);
            fout << std::endl;
        }
    }
    fout.close();
    return 0;
}