cmake_minimum_required(VERSION 3.16)
project(ancient)

set(CMAKE_CXX_STANDARD 20)
add_compile_options("-fms-extensions")

add_executable(ancient main.cpp arith_coding.c arith_coding.h)