cmake_minimum_required(VERSION 3.24)
project(.)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_compile_options(-Wall -g -O3 -fPIC)

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    add_compile_options(-fconstexpr-ops-limit=99999999)
elseif( CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$" )
    add_compile_options(-fconstexpr-steps=99999999)
endif()

file(GLOB_RECURSE INCLUDE "./src/*.hpp")
file(GLOB_RECURSE SOURCES "./src/*.cpp")

add_executable(brainiac ${SOURCES})
