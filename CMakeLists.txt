cmake_minimum_required(VERSION 3.13)
project(
    OpenHack 
    VERSION 1.6.0
    HOMEPAGE_URL "https://github.com/Prevter/GDOpenHack"
    LANGUAGES C CXX
)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Configure the project.h file
configure_file("src/project.h.in" "${CMAKE_BINARY_DIR}/project.h")
include_directories("${CMAKE_BINARY_DIR}")

# Get all files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Create the library
add_library(OpenHack SHARED ${SOURCES})

# Set the output directory
set_target_properties(OpenHack PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Add libraries
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLEW_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(libs/json)
add_subdirectory(libs/glfw)
add_subdirectory(libs/discord-rpc)

# Include STB
include_directories(libs/stb)

# Setup ImGui
add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/backends/imgui_impl_win32.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
    libs/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    libs/imgui
    libs/imgui/backends
)

target_link_libraries(imgui PUBLIC
    glfw
    opengl32
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Link libraries
target_include_directories(OpenHack PUBLIC
    libs/discord-rpc/include
)
target_link_libraries(OpenHack
    nlohmann_json::nlohmann_json
    opengl32
    glfw
    imgui
    discord-rpc
)

setup_geode_mod(${PROJECT_NAME})