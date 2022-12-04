#include "SymbolTable.h"
#include "definitions.h"

auto SymbolTable::startSubroutine() -> void
{
    // Clear the subroutine table at each new subroutine definition.
    mSubroutineTable.clear();
};

auto SymbolTable::define(std::string name, std::string type, Kind kind) -> void
{
    // Decide which table to use.
    auto &table = decideTable(kind);

    // Decide whether to insert or just increment counter.
    if (table.find(name) == table.end())
    {
        auto varCount = this->varCount(kind);
        table[name] = {.type = type, .kind = kind, .index = varCount};
    }
};

auto SymbolTable::decideTable(Kind kind) -> std::unordered_map<std::string, HashData> &
{
    // Decide which table to use.
    std::unordered_map<std::string, HashData> table;
    if (kind == Kind::STATIC || kind == Kind::FIELD)
        return mClassTable;
    else
        return mSubroutineTable;
};

auto SymbolTable::varCount(Kind kind) -> int
{
    auto table = decideTable(kind);
    int counter = 0;
    for (auto &it : table)
        if (it.second.kind == kind)
            counter++;
    return counter;
}

auto SymbolTable::kindOf(std::string name) -> Kind
{
    Kind kind = Kind::NONE;
    if (mClassTable.find(name) != mClassTable.end())
        kind = mClassTable[name].kind;
    else if (mSubroutineTable.find(name) != mSubroutineTable.end())
        kind = mSubroutineTable[name].kind;
    return kind;
};

auto SymbolTable::typeOf(std::string name) -> std::string
{
    std::string type = "";
    if (mClassTable.find(name) != mClassTable.end())
        type = mClassTable[name].type;
    else if (mSubroutineTable.find(name) != mSubroutineTable.end())
        type = mSubroutineTable[name].type;
    return type;
}

auto SymbolTable::indexOf(std::string name) -> int
{
    int index = -1;
    if (mClassTable.find(name) != mClassTable.end())
        index = mClassTable[name].index;
    else if (mSubroutineTable.find(name) != mSubroutineTable.end())
        index = mSubroutineTable[name].index;
    return index;
}