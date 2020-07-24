cmake_minimum_required(VERSION 3.16)
project(user)
set(CMAKE_C_COMPILER "clang-10")
set(CMAKE_CXX_COMPILER "clang++-10")
set(CMAKE_CXX_FLAGS  "-g -nostdlib -Wall")
set(CMAKE_C_FLAGS "-g -nostdlib -Wall")

add_executable(user main.cpp) 

add_custom_command(TARGET user
        POST_BUILD
        COMMAND cp ${CMAKE_BINARY_DIR}/user ${CMAKE_BINARY_DIR}/../build/iso/
        )