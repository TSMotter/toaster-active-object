cmake_minimum_required(VERSION 3.10)
project(my_cpp)

set(CMAKE_CXX_STANDARD 14)


# Variable TARGET_GROUP should be passed as an argument when calling cmake
set(TARGET_GROUP helloworld CACHE STRING "Specify the TARGET_GROUP?")

if(TARGET_GROUP STREQUAL DemoThreadSafeQueue)
  add_subdirectory(ThreadSafeQueue)
elseif(TARGET_GROUP STREQUAL DemoActiveObject)
  add_subdirectory(ActiveObject)
endif()

add_subdirectory("${TARGET_GROUP}")
