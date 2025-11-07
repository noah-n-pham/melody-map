cmake_minimum_required(VERSION 3.20)
project(melody_map LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use FetchContent to automatically download and build SFML (more portable)
include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_MakeAvailable(SFML)

# Add both source files
add_executable(melody_map 
    gui.cpp
    data_parse.cpp
)

# Include SFML headers
target_include_directories(melody_map PRIVATE
    ${SFML_SOURCE_DIR}/include
    ${SFML_BINARY_DIR}/include
)

# Link SFML libraries
target_link_libraries(melody_map
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)
