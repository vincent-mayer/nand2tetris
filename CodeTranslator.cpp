#include "CodeTranslator.h"
#include "detail/translation_maps.h"

CodeTranslator::CodeTranslator()
    : m_destMap(getDestMap())
    , m_compMap(getCompMap())
    , m_jumpMap(getJumpMap()){};

auto CodeTranslator::dest(std::string& dest) -> std::string
{
    return m_destMap[dest];
}

auto CodeTranslator::comp(std::string& comp) -> std::string
{
    return m_compMap[comp];
}

auto CodeTranslator::jump(std::string& jump) -> std::string
{
    return m_jumpMap[jump];
}