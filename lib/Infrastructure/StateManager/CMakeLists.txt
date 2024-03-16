add_library(StateManager INTERFACE)

target_sources(StateManager INTERFACE StateManager.h)

# Make the directory known
target_include_directories(StateManager INTERFACE ${CMAKE_SOURCE_DIR}/external)


# Link library to a binary target
target_link_libraries(StateManager INTERFACE tree)
