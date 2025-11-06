cmake_minimum_required(VERSION 3.20)
project(melody_map)

set(CMAKE_CXX_STANDARD 17)

# Path to your SFML 3 installation
set(SFML_DIR "C:/Program Files/SFML-3.0.2/lib/cmake/SFML")

# Find SFML 3 (minimum version 3.0.2)
find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System)

# Add your source file
add_executable(melody_map 
    "C:/Users/Minh Tran/Desktop/melody map/melody-map/gui.cpp"
)

# Link SFML libraries (note the correct target name here)
target_link_libraries(melody_map
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Copy necessary SFML DLLs to the build directory after building
if(WIN32)
    add_custom_command(TARGET melody_map POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE_DIR:melody_map>
    )
endif()
