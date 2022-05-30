cmake_minimum_required(VERSION 3.10)
project(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -march=native -m64 -O3")

# Engine source files
file(GLOB_RECURSE INCLUDE "./src/*.h")
file(GLOB_RECURSE SOURCES "./src/*.cpp" "./src/*.c")

# Make the Brainiac engine a library
add_library(brainiac STATIC ${SOURCES} ${INCLUDE})
target_include_directories(brainiac PUBLIC "./src")
 
add_executable(main "./src/main.cpp")

if(UNIX OR MSVC)
    target_link_libraries(main pthread brainiac)
endif()

if(WIN32) 
    target_link_libraries(main brainiac)
endif()