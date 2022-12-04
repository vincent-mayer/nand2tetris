#pragma once

#include "definitions.h"
#include <string>
#include <unordered_map>

class SymbolTable
{
private:
    int i;
    std::unordered_map<std::string, HashData> mClassTable;
    std::unordered_map<std::string, HashData> mSubroutineTable;

public:
    SymbolTable(){};

    auto startSubroutine() -> void;

    auto define(std::string name, std::string type, Kind kind) -> void;

    auto decideTable(Kind kind) -> std::unordered_map<std::string, HashData> &;

    auto varCount(Kind kind) -> int;

    auto kindOf(std::string name) -> Kind;

    auto typeOf(std::string name) -> std::string;

    auto indexOf(std::string name) -> int;
};