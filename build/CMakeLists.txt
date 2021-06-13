cmake_minimum_required(VERSION 3.10)
project(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -O3")

# Engine source files
file(GLOB_RECURSE INCLUDE "../engine/*.h")
file(GLOB_RECURSE SOURCES "../engine/*.cpp" "../engine/*.c")

# Make the Brainiac engine a shared library
add_library(Brainiac ${SOURCES} ${INCLUDE})

add_executable(main "../main.cpp")

# Compile on Linux systems
if(UNIX OR MSVC)
    target_link_libraries(main Brainiac)
endif()
