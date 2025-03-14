cmake_minimum_required(VERSION 3.31)
project(DirSize)

set(CMAKE_CXX_STANDARD 23)

add_executable(DirSize main.cpp)

target_compile_definitions(DirSize
        PRIVATE DSIZE_VERSION="1.0"
)