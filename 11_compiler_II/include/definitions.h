#pragma once

#include <map>
#include <string>
#include <vector>

enum class FunctionType
{
    CONSTRUCTOR,
    METHOD,
    FUNCTION
};

enum class Category
{
    VAR,
    ARGUMENT,
    STATIC,
    FIELD,
    CLASS,
    SUBROUTINE
};

enum class Kind
{
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
};

enum class Segment
{
    CONST,
    ARG,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

inline std::map<Kind, Segment> kindToSegment{
    {Kind::VAR, Segment::LOCAL},
    {Kind::ARG, Segment::ARG},
    {Kind::FIELD, Segment::THIS},
};

inline std::map<Segment, std::string> segmentToString{
    {Segment::CONST, std::string{"constant"}},  {Segment::ARG, std::string{"argument"}},
    {Segment::LOCAL, std::string{"local"}},     {Segment::STATIC, std::string{"static"}},
    {Segment::THIS, std::string{"this"}},       {Segment::THAT, std::string{"that"}},
    {Segment::POINTER, std::string{"pointer"}}, {Segment::TEMP, std::string{"temp"}},
};

enum class Command
{
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

inline std::map<std::string, std::string> commandToString{
    {std::string{"+"}, std::string{"add"}},
    {std::string{"-"}, std::string{"sub"}},
    {std::string{"neg"}, std::string{"neg"}},
    {std::string{"="}, std::string{"eq"}},
    {std::string{">"}, std::string{"gt"}},
    {std::string{"<"}, std::string{"lt"}},
    {std::string{"&"}, std::string{"and"}},
    {std::string{"|"}, std::string{"or"}},
    {std::string{"~"}, std::string{"not"}},
    {std::string{"not"}, std::string{"not"}},
    {std::string{"*"}, std::string{"call Math.multiply 2"}},
    {std::string{"/"}, std::string{"call Math.divide 2"}},
};

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

struct HashData
{
    std::string type;
    Kind kind;
    int index;
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
