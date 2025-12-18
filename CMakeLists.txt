cmake_minimum_required(VERSION 3.31)
project(Arcadia_Engine)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Arcadia_Engine
        ArcadiaEngine.cpp
        ArcadiaEngine.h
        main_test_students.cpp)
