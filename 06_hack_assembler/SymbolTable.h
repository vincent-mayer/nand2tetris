#include <map>
#include <string>

class SymbolTable
{
public:
    SymbolTable();

    auto addEntry(const std::string& string, const int address) -> void;

    auto contains(const std::string& symbol) -> bool;

    auto isPredefined(const std::string& symbol) -> bool;

    auto getAddress(const std::string& string) -> int;

    auto printTable() -> void;

private:
    std::map<std::string, int> m_table;
};