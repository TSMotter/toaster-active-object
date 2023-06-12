# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)

# Add a cmake binary taget (in this case, an executable)
add_executable(main main.cpp)

# Make the directory known
target_include_directories(main PUBLIC ${Boost_INCLUDE_DIR})
# Link library to a binary target
target_link_libraries(main PUBLIC ${Boost_LIBRARIES})