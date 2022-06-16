#include <JackTokenizer.hpp>

auto JackTokenizer::advance() -> void{};

auto JackTokenizer::tokenType() -> TokenType{};

auto JackTokenizer::keyWord() -> KeyWord{};

auto JackTokenizer::symbol() -> char{};

auto JackTokenizer::identifier() -> std::string{};

auto JackTokenizer::intVal() -> int{};

auto JackTokenizer::stringVal() -> std::string{};