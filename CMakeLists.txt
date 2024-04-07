cmake_minimum_required(VERSION 3.24)
project(.)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_compile_options(-Wall -g -O3 -fPIC -fconstexpr-steps=99999999)

file(GLOB_RECURSE INCLUDE "./src/*.hpp")
file(GLOB_RECURSE SOURCES "./src/*.cpp")

add_executable(brainiac ${SOURCES})
