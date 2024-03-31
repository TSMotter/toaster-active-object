# Add a cmake binary taget (in this case, a library)
add_library(Actuators Actuators.cpp Actuators.hpp)

target_include_directories(Actuators PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/DemoObjects
    ${CMAKE_SOURCE_DIR}/lib/Infrastructure/BoostDeadlineTimer)


target_link_libraries(Actuators PUBLIC
    DemoObjects
    BoostDeadlineTimer)