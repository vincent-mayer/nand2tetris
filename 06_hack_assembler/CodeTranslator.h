#include <map>
#include <string>

class CodeTranslator
{
public:
    CodeTranslator();

    auto dest(std::string& dest) -> std::string;

    auto comp(std::string& comp) -> std::string;

    auto jump(std::string& jump) -> std::string;

private:
    std::map<std::string, std::string> m_destMap;
    std::map<std::string, std::string> m_compMap;
    std::map<std::string, std::string> m_jumpMap;
};