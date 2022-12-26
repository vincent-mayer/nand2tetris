#include "VMWriter.h"
#include "definitions.h"
#include <iostream>

auto VMWriter::writePush(Segment segment, int index) -> void
{
    mOutputFile << "push ";
    mOutputFile << segmentToString[segment] << " ";
    mOutputFile << std::to_string(index) << std::endl;
};

auto VMWriter::writePop(Segment segment, int index) -> void
{
    mOutputFile << "pop ";
    mOutputFile << segmentToString[segment] << " ";
    mOutputFile << std::to_string(index) << std::endl;
};

auto VMWriter::writeArithmetic(std::string command) -> void
{
    mOutputFile << commandToString[command] << std::endl;
};

auto VMWriter::writeLabel(std::string label) -> void
{
    mOutputFile << "label " << label << std::endl;
};

auto VMWriter::writeGoto(std::string label) -> void
{
    mOutputFile << "goto " << label << std::endl;
};

auto VMWriter::writeIf(std::string label) -> void
{
    mOutputFile << "if-goto " << label << std::endl;
};

auto VMWriter::writeCall(std::string name, int nArgs) -> void
{
    mOutputFile << "call ";
    mOutputFile << name;
    mOutputFile << " " << std::to_string(nArgs) << std::endl;
};

auto VMWriter::writeFunction(std::string className, std::string functionName,
                             int nLocals) -> void
{
    mOutputFile << "function ";
    mOutputFile << className << "." << functionName << " ";
    mOutputFile << std::to_string(nLocals) << std::endl;
};

auto VMWriter::writeReturn() -> void
{
    mOutputFile << "return" << std::endl;
};
