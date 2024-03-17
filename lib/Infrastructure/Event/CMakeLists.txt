find_package(Boost 1.74.0 REQUIRED)

# Add a cmake binary taget (in this case, a library)
add_library(Event INTERFACE)
target_sources(Event INTERFACE Event.h)

# Make the directory known
target_include_directories(Event INTERFACE ${Boost_INCLUDE_DIR})
# Link library to a binary target
target_link_libraries(Event INTERFACE ${Boost_LIBRARIES})