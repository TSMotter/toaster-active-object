# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)

# ******************************************************************************
# **** Creates binary targets ****
# ******************************************************************************
# Add a cmake binary taget (in this case, libraries)
add_library(Actuators INTERFACE Actuators.hpp)
add_library(Sensors INTERFACE Sensors.hpp)
add_library(Events
            Events.cpp
            Events.hpp)
add_library(ToasterActiveObject
            ToasterActiveObject.cpp
            ToasterActiveObject.hpp)

# ******************************************************************************
# **** Make all other directories known to this one ****
# ******************************************************************************
target_include_directories(ToasterActiveObject PUBLIC 
                            ${Boost_INCLUDE_DIR}
                            ${CMAKE_SOURCE_DIR}/lib/Infrastructure/ThreadSafeQueue
                            ${CMAKE_SOURCE_DIR}/lib/Infrastructure/BoostDeadlineTimer)

# ******************************************************************************
# **** Link the libraries ****
# ******************************************************************************
target_link_libraries(ToasterActiveObject PUBLIC
                        ${Boost_LIBRARIES}
                        Actuators
                        Sensors
                        Events
                        ThreadSafeQueue
                        BoostDeadlineTimer)