#include <map>
#include <string>

auto getDestMap() -> std::map<std::string, std::string>
{
    std::map<std::string, std::string> dest;
    dest[""]    = "000";
    dest["M"]   = "001";
    dest["D"]   = "010";
    dest["MD"]  = "011";
    dest["A"]   = "100";
    dest["AM"]  = "101";
    dest["AD"]  = "110";
    dest["AMD"] = "111";
    return dest;
}
auto getCompMap() -> std::map<std::string, std::string>
{
    std::map<std::string, std::string> comp;
    comp["0"]   = "0101010";
    comp["1"]   = "0111111";
    comp["-1"]  = "0111010";
    comp["D"]   = "0001100";
    comp["A"]   = "0110000";
    comp["!D"]  = "0001101";
    comp["!A"]  = "0110001";
    comp["-D"]  = "0001111";
    comp["-A"]  = "0110011";
    comp["D+1"] = "0011111";
    comp["A+1"] = "0110111";
    comp["D-1"] = "0001110";
    comp["A-1"] = "0110010";
    comp["D+A"] = "0000010";
    comp["D-A"] = "0010011";
    comp["A-D"] = "0000111";
    comp["D&A"] = "0000000";
    comp["D|A"] = "0010101";
    comp["M"]   = "1110000";
    comp["!M"]  = "1110001";
    comp["-M"]  = "1110011";
    comp["M+1"] = "1110111";
    comp["M-1"] = "1110010";
    comp["D+M"] = "1000010";
    comp["D-M"] = "1010011";
    comp["M-D"] = "1000111";
    comp["D&M"] = "1000000";
    comp["D|M"] = "1010101";
    return comp;
}
auto getJumpMap() -> std::map<std::string, std::string>
{
    std::map<std::string, std::string> jump;
    jump[""]    = "000";
    jump["JGT"] = "001";
    jump["JEQ"] = "010";
    jump["JGE"] = "011";
    jump["JLT"] = "100";
    jump["JNE"] = "101";
    jump["JLE"] = "110";
    jump["JMP"] = "111";
    return jump;
}