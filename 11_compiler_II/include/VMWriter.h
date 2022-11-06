#pragma once

#include "definitions.h"
#include <fstream>
#include <string>

class VMWriter
{
private:
    std::ofstream mOutputFile;

public:
    VMWriter(std::string filename) : mOutputFile(filename){};

    ~VMWriter()
    {
        if (mOutputFile.is_open())
            mOutputFile.close();
    };

    auto writePush(Segment segment, int index) -> void;

    auto writePop(Segment segment, int index) -> void;

    auto writeArithmetic(Command command) -> void;

    auto writeLabel(std::string label) -> void;

    auto writeGoto(std::string label) -> void;

    auto writeIf(std::string label) -> void;

    auto writeCall(std::string name, int nArgs) -> void;

    auto writeFunction(std::string name, int nLocals) -> void;

    auto writeReturn() -> void;

    auto close() -> void
    {
        if (mOutputFile.is_open())
            mOutputFile.close();
    };
};
