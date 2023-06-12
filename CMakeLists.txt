cmake_minimum_required(VERSION 3.11)
project(my_cpp)

set(CMAKE_CXX_STANDARD 17)

# Variable TARGET_GROUP should be passed as an argument when calling cmake
set(TARGET_GROUP helloworld CACHE STRING "Specify the TARGET_GROUP?")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(lib)
if(TARGET_GROUP STREQUAL test)
    add_subdirectory(test)
else()
    add_subdirectory(src)
endif()

