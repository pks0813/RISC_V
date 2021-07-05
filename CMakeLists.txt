cmake_minimum_required(VERSION 3.15)
project(code)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-Ofast)

add_executable(code
        RS.hpp
        Instruction.hpp
        LSBuffer.hpp
        register.hpp
        Queue.hpp
        ROB.hpp
        include.hpp
        tools.hpp
        EX.hpp
        main.cpp
        CPU.hpp)
