cmake_minimum_required(VERSION 3.13)  # CMake version check
project(JackAnalyzer)
               
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
    include/JackTokenizer.h
    include/definitions.h
    include/CompilationEngine.h
    src/JackTokenizer.cpp
    src/CompilationEngine.cpp
    src/JackAnalyzer.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(JackAnalyzer ${SOURCE_FILES})
target_include_directories(JackAnalyzer PRIVATE include/)