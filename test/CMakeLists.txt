set(UNIT_TESTS_CMAKE_TARGET "main")

# Fetches gtest release 1.13.0. To get later release, update the commit hash to download
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/b796f7d.zip)
FetchContent_MakeAvailable(googletest)

# Enables testing in CMake
enable_testing()

# Define cmake binary taget (in this case, an executable)
add_executable(${UNIT_TESTS_CMAKE_TARGET}
    testintegers.cpp
    teststrings.cpp
)

# Link library to the binary target. GTest::gtest_main offers me a default main() function
target_link_libraries(${UNIT_TESTS_CMAKE_TARGET} GTest::gtest_main)

# Enable CMake’s test runner to discover the tests included in the binary
include(GoogleTest)
gtest_discover_tests(${UNIT_TESTS_CMAKE_TARGET})