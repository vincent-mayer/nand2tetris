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

    auto define(std::string const &name, std::string const &type, Kind const &kind) -> void;

    auto decideTable(Kind const &kind) -> std::unordered_map<std::string, HashData> &;

    auto varCount(Kind const &kind) -> int;

    auto kindOf(std::string const &name) -> Kind;

    auto typeOf(std::string const &name) -> std::string;

    auto indexOf(std::string const &name) -> int;
};