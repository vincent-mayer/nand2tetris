cmake_minimum_required(VERSION 3.10)

project(Assembler VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES 
    main.cpp
    SymbolTable.h
    SymbolTable.cpp
    CodeTranslator.h
    CodeTranslator.cpp
    Parser.h
    Parser.cpp)

add_executable(assemble
               ${SOURCE_FILES})
               