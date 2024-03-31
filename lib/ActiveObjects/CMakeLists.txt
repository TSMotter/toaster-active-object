# Find necessary packages
find_package(Boost 1.74.0 REQUIRED)

# ******************************************************************************
# **** Creates binary targets ****
# ******************************************************************************
# Add a cmake binary taget (in this case, a library)
add_library(Toaster
            Toaster.cpp
            Toaster.hpp)

# ******************************************************************************
# **** Make all other directories known to this one ****
# ******************************************************************************
target_include_directories(Toaster PUBLIC 
                            ${Boost_INCLUDE_DIR}
                            ${CMAKE_SOURCE_DIR}/lib/Actuators
                            ${CMAKE_SOURCE_DIR}/lib/Sensors
                            ${CMAKE_SOURCE_DIR}/lib/Infrastructure/StateManager
                            ${CMAKE_SOURCE_DIR}/lib/Infrastructure/ThreadSafeQueue)

# ******************************************************************************
# **** Link the libraries ****
# ******************************************************************************
target_link_libraries(Toaster PUBLIC
                        ${Boost_LIBRARIES}
                        Actuators
                        Sensors
                        StateManager
                        ThreadSafeQueue)