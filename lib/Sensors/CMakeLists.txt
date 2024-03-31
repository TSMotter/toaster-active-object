# Add a cmake binary taget (in this case, a library)
add_library(Sensors Sensors.cpp Sensors.hpp)

target_include_directories(Sensors PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/DemoObjects
    ${CMAKE_SOURCE_DIR}/lib/Infrastructure/Event
    ${CMAKE_SOURCE_DIR}/lib/Infrastructure/BoostDeadlineTimer)


target_link_libraries(Sensors PUBLIC
    DemoObjects
    Event
    BoostDeadlineTimer)