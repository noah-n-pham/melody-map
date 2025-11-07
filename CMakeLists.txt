cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(melody_map 
        data_parse.cpp
        rNN.cpp
        gui.cpp
        )

# adds the dataset to the cwd
add_custom_command(
    TARGET melody_map
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dataset.csv  
        $<TARGET_FILE_DIR:melody_map>    
    COMMENT "Copying dataset.csv to executable directory"
)
target_include_directories(melody_map PRIVATE
    ${SFML_SOURCE_DIR}/include
    ${SFML_BINARY_DIR}/include
)

target_link_libraries(melody_map PRIVATE 
        SFML::Graphics
        SFML::Window
        SFML::System
        )