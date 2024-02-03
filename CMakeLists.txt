cmake_minimum_required(VERSION 3.13)
project(
    OpenHack 
    VERSION 1.6.1
    HOMEPAGE_URL "https://github.com/Prevter/GDOpenHack"
    LANGUAGES C CXX
)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Configure the project.h file
configure_file("src/project.h.in" "${CMAKE_BINARY_DIR}/project.h")
include_directories("${CMAKE_BINARY_DIR}")

# Get all files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# Create the library
add_library(OpenHack SHARED ${SOURCES} "src/injector/xinput.def")

# Set the output directory
set_target_properties(OpenHack PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Set the output name
set_target_properties(OpenHack PROPERTIES OUTPUT_NAME "xinput9_1_0")

# Add libraries
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLEW_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(libs/json)
add_subdirectory(libs/minhook)
add_subdirectory(libs/glfw)
add_subdirectory(libs/cocos-headers)
add_subdirectory(libs/spdlog)
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
target_include_directories(OpenHack PRIVATE 
    libs/discord-rpc/include
    libs/cocos-headers/cocos2dx/platform/third_party/win32
    libs/imgui_markdown
)
target_link_libraries(OpenHack PRIVATE
    nlohmann_json::nlohmann_json
    minhook
    cocos2d
    opengl32
    glfw
    imgui
    spdlog::spdlog
    discord-rpc
    ${CMAKE_SOURCE_DIR}/libs/libcurl.lib
)