#include "VMWriter.h"
#include "definitions.h"
#include <iostream>

auto VMWriter::writePush(Segment segment, int index) -> void
{
    std::cout << "push ";
    std::cout << segmentToString[segment] << " ";
    std::cout << std::to_string(index) << std::endl;
};

auto VMWriter::writePop(Segment segment, int index) -> void
{
    std::cout << "pop ";
    std::cout << segmentToString[segment] << " ";
    std::cout << std::to_string(index) << std::endl;
};

auto VMWriter::writeArithmetic(std::string command) -> void
{
    std::cout << commandToString[command] << std::endl;
};

auto VMWriter::writeLabel(std::string label) -> void{

};

auto VMWriter::writeGoto(std::string label) -> void{

};

auto VMWriter::writeIf(std::string label) -> void{

};

auto VMWriter::writeCall(std::string name, int nArgs) -> void
{
    std::cout << "call ";
    std::cout << name;
    std::cout << " " << std::to_string(nArgs) << std::endl;
};

auto VMWriter::writeFunction(std::string className, std::string functionName,
                             int nLocals) -> void
{
    std::cout << "function ";
    std::cout << className << "." << functionName << " ";
    std::cout << std::to_string(nLocals) << std::endl;
};

auto VMWriter::writeReturn() -> void
{
    std::cout << "return" << std::endl;
};
