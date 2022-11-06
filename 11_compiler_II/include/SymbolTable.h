#pragma once

#include "definitions.h"
#include <string>

class SymbolTable
{
private:
    int i;

public:
    SymbolTable();

    auto startSubroutine(std::string name, std::string type, Kind kind) -> void;

    auto varCount(Kind kind) -> int;

    auto kindOf(std::string name) -> Kind;

    auto typeOf(std::string name) -> std::string;

    auto indexOf(std::string name) -> int;
};