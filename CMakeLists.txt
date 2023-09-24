cmake_minimum_required(VERSION 3.11)
project(my_cpp)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Variable TARGET_GROUP should be passed as an argument when calling cmake
set(TARGET_GROUP helloworld CACHE STRING "Specify the TARGET_GROUP?")

add_subdirectory(lib)
add_subdirectory("${TARGET_GROUP}")