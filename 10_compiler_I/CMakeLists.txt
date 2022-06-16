cmake_minimum_required(VERSION 3.13)  # CMake version check
project(JackTokenizer)
               
set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
    src/main.cpp
    include/JackTokenizer.h
    include/definitions.h
    src/JackTokenizer.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(JackTokenizer ${SOURCE_FILES})
target_include_directories(JackTokenizer PRIVATE include/)