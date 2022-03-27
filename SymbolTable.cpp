#include "SymbolTable.h"
#include <algorithm>
#include <iostream>
#include <vector>

std::vector<std::string> PDS = {"SP", "LCL", "ARG", "THIS", "THAT", "R0",  "R1",  "R2",  "R3",  "R4",     "R5", "R6",
                                "R7", "R8",  "R9",  "R10",  "R11",  "R12", "R13", "R14", "R15", "SCREEN", "KBD"};

SymbolTable::SymbolTable()
{
    m_table["SP"]     = 0;
    m_table["LCL"]    = 1;
    m_table["ARG"]    = 2;
    m_table["THIS"]   = 3;
    m_table["THAT"]   = 4;
    m_table["R0"]     = 0;
    m_table["R1"]     = 1;
    m_table["R2"]     = 2;
    m_table["R3"]     = 3;
    m_table["R4"]     = 4;
    m_table["R5"]     = 5;
    m_table["R6"]     = 6;
    m_table["R7"]     = 7;
    m_table["R8"]     = 8;
    m_table["R9"]     = 9;
    m_table["R10"]    = 10;
    m_table["R11"]    = 11;
    m_table["R12"]    = 12;
    m_table["R13"]    = 13;
    m_table["R14"]    = 14;
    m_table["R15"]    = 15;
    m_table["SCREEN"] = 16384;
    m_table["KBD"]    = 24576;
};

auto SymbolTable::printTable() -> void
{
    std::cout << "SymbolTable" << std::endl;
    for (auto& [k, v] : m_table) {
        std::cout << k << ": " << v << "\n";
    }
}

auto SymbolTable::isPredefined(const std::string& symbol) -> bool
{
    if (std::find(PDS.begin(), PDS.end(), symbol) != PDS.end())
        return true;
    else
        return false;
}

auto SymbolTable::addEntry(const std::string& symbol, const int address) -> void
{
    if (contains(symbol)) {
        std::cout << "Symbol is already in table!" << std::endl;
    }
    else {
        m_table[symbol] = address;
    }
}

auto SymbolTable::contains(const std::string& symbol) -> bool
{
    if (m_table.count(symbol))
        return true;
    else
        return false;
}

auto SymbolTable::getAddress(const std::string& symbol) -> int
{
    if (contains(symbol)) {
        return m_table[symbol];
    }
    else {
        std::cout << "Symbol is not in table!" << std::endl;
        return -1;
    }
}