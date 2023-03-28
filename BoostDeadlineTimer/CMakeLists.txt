# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)
find_package(spdlog 1.9.0 REQUIRED)

# Add a cmake binary taget (in this case, a library)
add_library(BoostDeadlineTimer BoostDeadlineTimer.cpp BoostDeadlineTimer.hpp)

# Make the directory known
target_include_directories(BoostDeadlineTimer PUBLIC ${Boost_INCLUDE_DIR})
# Link library to a binary target
target_link_libraries(BoostDeadlineTimer PUBLIC ${Boost_LIBRARIES})

# Link library to a binary target
target_link_libraries(BoostDeadlineTimer PRIVATE spdlog::spdlog)