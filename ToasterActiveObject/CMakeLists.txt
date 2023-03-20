# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)

# Add a cmake binary taget (in this case, a library)
add_library(ToasterActiveObject ToasterActiveObject.cpp ToasterActiveObject.hpp)
target_include_directories(ToasterActiveObject PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(ToasterActiveObject PUBLIC ${Boost_LIBRARIES})