cmake_minimum_required(VERSION 3.13)  # CMake version check

project(JackCompiler)

set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard


# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
    include/CompilationEngine.h
    include/definitions.h
    include/JackTokenizer.h
    include/SymbolTable.h
    include/VMWriter.h
    src/CompilationEngine.cpp
    src/JackAnalyzer.cpp
    src/JackCompiler.cpp
    src/JackTokenizer.cpp
    src/SymbolTable.cpp
    src/VMWriter.cpp
    )

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(JackCompiler ${SOURCE_FILES})
target_include_directories(JackCompiler PRIVATE include/ /home/vincent/repos/nand2tetris/10_compiler_I/include)
