# Find necessary packages
find_package(spdlog 1.9.0 REQUIRED)

# Add a cmake binary taget (in this case, a library)
add_library(ThreadSafeQueue ThreadSafeQueue.cpp ThreadSafeQueue.hpp)
# Link library to a binary target
target_link_libraries(ThreadSafeQueue PRIVATE spdlog::spdlog)
