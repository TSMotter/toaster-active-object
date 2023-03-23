# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)

# Add a cmake binary taget (in this case, a library)
add_library(Events Events.cpp Events.hpp)
# Add a cmake binary taget (in this case, a library)
add_library(ToasterActiveObject ToasterActiveObject.cpp ToasterActiveObject.hpp)

# Link library to a binary target
target_link_libraries(ToasterActiveObject PUBLIC Events)

# Make the directory known
target_include_directories(ToasterActiveObject PUBLIC ${CMAKE_SOURCE_DIR}/ThreadSafeQueue)
# Link library to a binary target
target_link_libraries(ToasterActiveObject PUBLIC ThreadSafeQueue)

# Make the directory known
target_include_directories(ToasterActiveObject PUBLIC ${Boost_INCLUDE_DIR})
# Link library to a binary target
target_link_libraries(ToasterActiveObject PUBLIC ${Boost_LIBRARIES})