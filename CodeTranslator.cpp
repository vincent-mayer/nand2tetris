#include "CodeTranslator.h"

CodeTranslator::CodeTranslator()
{
    m_destMap["null"] = "000";
    m_destMap["M"]    = "001";
    m_destMap["D"]    = "010";
    m_destMap["MD"]   = "011";
    m_destMap["A"]    = "100";
    m_destMap["AM"]   = "101";
    m_destMap["AD"]   = "110";
    m_destMap["AMD"]  = "111";
    m_compMap["0"]    = "0101010";
    m_compMap["1"]    = "0111111";
    m_compMap["-1"]   = "0111010";
    m_compMap["D"]    = "0001100";
    m_compMap["A"]    = "0110000";
    m_compMap["!D"]   = "0001101";
    m_compMap["!A"]   = "0110001";
    m_compMap["-D"]   = "0001111";
    m_compMap["-A"]   = "0110011";
    m_compMap["D+1"]  = "0011111";
    m_compMap["A+1"]  = "0110111";
    m_compMap["D-1"]  = "0001110";
    m_compMap["A-1"]  = "0110010";
    m_compMap["D+A"]  = "0000010";
    m_compMap["D-A"]  = "0010011";
    m_compMap["A-D"]  = "0000111";
    m_compMap["D&A"]  = "0000000";
    m_compMap["D|A"]  = "0010101";
    m_compMap["M"]    = "1110000";
    m_compMap["!M"]   = "1110001";
    m_compMap["-M"]   = "1110011";
    m_compMap["M+1"]  = "1110111";
    m_compMap["M-1"]  = "1110010";
    m_compMap["D+M"]  = "1000010";
    m_compMap["D-M"]  = "1010011";
    m_compMap["M-D"]  = "1000111";
    m_compMap["D&M"]  = "1000000";
    m_compMap["D|M"]  = "1010101";
    m_jumpMap["null"] = "000";
    m_jumpMap["JGT"]  = "001";
    m_jumpMap["JEQ"]  = "010";
    m_jumpMap["JGE"]  = "011";
    m_jumpMap["JLT"]  = "100";
    m_jumpMap["JNE"]  = "101";
    m_jumpMap["JLE"]  = "110";
    m_jumpMap["JMP"]  = "111";
};

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