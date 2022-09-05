#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <map>
#include <string>
#include <vector>

enum class TokenType
{
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum class KeyWord
{
    CLASS,
    CONSTRUCTOR,
    FUNCTION,
    METHOD,
    FIELD,
    STATIC,
    VAR,
    INT,
    CHAR,
    BOOLEAN,
    VOID,
    TRUE,
    FALSE,
    NULL_,
    THIS,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
};

inline std::map<std::string, KeyWord> KEYWORD_MAP{
    {"class", KeyWord::CLASS},
    {"constructor", KeyWord::CONSTRUCTOR},
    {"function", KeyWord::FUNCTION},
    {"method", KeyWord::METHOD},
    {"field", KeyWord::FIELD},
    {"static", KeyWord::STATIC},
    {"var", KeyWord::VAR},
    {"int", KeyWord::INT},
    {"char", KeyWord::CHAR},
    {"boolean", KeyWord::BOOLEAN},
    {"void", KeyWord::VOID},
    {"true", KeyWord::TRUE},
    {"false", KeyWord::FALSE},
    {"null", KeyWord::NULL_},
    {"this", KeyWord::THIS},
    {"let", KeyWord::LET},
    {"do", KeyWord::DO},
    {"if", KeyWord::IF},
    {"else", KeyWord::ELSE},
    {"while", KeyWord::WHILE},
};

inline std::vector<std::string> ops{"+", "-", "*", "/", "&", "<",
                                    ">", "=", "|", ",", "~"};

inline std::vector<std::string> tokens{
    "class", "constructor", "function", "method", "field", "static", "var",  "int",
    "char",  "boolean",     "void",     "true",   "false", "null",   "this", "let",
    "do",    "if",          "else",     "while",  "{",     "}",      "(",    ")",
    "[",     "]",           ".",        ",",      ";",     "+",      "-",    "*",
    "/",     "&",           "|",        "<",      ">",     "=",      "~",
};
// inline std::vector<std::string> char_tokens{
//     "{", "}", "(", ")", "[", "]", ".", ",", ";", "+",
//     "-", "*", "/", "&", "|", "<", ">", "=", "~",
// };

inline std::vector<std::string> keywords{
    "class", "constructor", "function", "method", "field", "static", "var",
    "int",   "char",        "boolean",  "void",   "true",  "false",  "null",
    "this",  "let",         "do",       "if",     "else",  "while",  "return"};

inline std::vector<std::string> symbols{"{", "}", "(", ")", "[", "]", ".",
                                        ",", ";", "+", "-", "*", "/", "&",
                                        ",", "<", ">", "=", "~", "|"};

#endif