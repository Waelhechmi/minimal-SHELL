cmake_minimum_required(VERSION 3.10)
project(SimpleCMakeProject)

set(CMAKE_CXX_STANDARD 17)
#include_directories(${CMAKE_SOURCE_DIR})

add_executable(my_shell shellc.c)
