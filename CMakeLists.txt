cmake_minimum_required(VERSION 3.10)
project(AsteroidPatrol)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define library paths
# Use environment variable or fallback to default path
if(DEFINED ENV{GAME_LIBS_PATH})
    set(LIB_PATH $ENV{GAME_LIBS_PATH})
else()
    # Default fallback - users should set GAME_LIBS_PATH environment variable
    # or modify this path to match their local setup
    set(LIB_PATH "C:/Users/shesh/source/repos/Libraries")
    message(WARNING "GAME_LIBS_PATH environment variable not set. Using default: ${LIB_PATH}")
    message(WARNING "To customize: Set GAME_LIBS_PATH environment variable to your Libraries folder")
endif()

set(INCLUDE_PATH "${LIB_PATH}/include")
set(LIB_BINARIES "${LIB_PATH}/lib")

# Add include directories
include_directories(${INCLUDE_PATH})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Collect source files
file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ui/*.cpp
)

# Create executable
add_executable(AsteroidPatrol ${SOURCES})

# Link libraries
target_link_libraries(AsteroidPatrol
    ${OPENGL_LIBRARIES}
    ${LIB_BINARIES}/glew32s.lib
    ${LIB_BINARIES}/glfw3.lib
    gdi32
    user32
    shell32
)

# Add compile definitions
target_compile_definitions(AsteroidPatrol PRIVATE
    GLEW_STATIC
    GLM_FORCE_RADIANS
    _CRT_SECURE_NO_WARNINGS
)

# Set output directory
set_target_properties(AsteroidPatrol PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin
)

# Copy shader files to build directory and bin directory
add_custom_command(TARGET AsteroidPatrol POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMENT "Copying shader files to build directory..."
)

add_custom_command(TARGET AsteroidPatrol POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/shaders
        ${PROJECT_SOURCE_DIR}/bin/shaders
    COMMENT "Copying shader files to bin directory..."
)

# Copy assets/shaders (text rendering shaders) to bin directory
add_custom_command(TARGET AsteroidPatrol POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets/shaders
        ${PROJECT_SOURCE_DIR}/bin/shaders
    COMMENT "Copying UI shader files to bin directory..."
)

# Print configuration info
message(STATUS "=== Asteroid Patrol Configuration ===")
message(STATUS "Library path: ${LIB_PATH}")
message(STATUS "Include path: ${INCLUDE_PATH}")
message(STATUS "Binary path: ${LIB_BINARIES}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "======================================")
