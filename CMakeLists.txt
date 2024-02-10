cmake_minimum_required(VERSION 3.15)
project(
    OpenHack 
    VERSION 2.0.0 
    HOMEPAGE_URL "https://github.com/Prevter/GDOpenHack"
    LANGUAGES C CXX
)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set the default build type to RelWithDebInfo
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build" FORCE)
endif()

# Configure the project version
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

# ========================================
# | Shared files
# ========================================
file(
    GLOB_RECURSE SHARED_SOURCE_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/src/shared/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shared/*.hpp
)

add_library(
    ${PROJECT_NAME}
    INTERFACE
    "${CMAKE_BINARY_DIR}/version.h"
)

add_subdirectory(libs)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    ${CMAKE_BINARY_DIR}
    libs/glew/include
)

target_link_libraries(
    ${PROJECT_NAME}
    INTERFACE
    external_libs
    glfw
    opengl32
    ${CMAKE_SOURCE_DIR}/libs/glew/lib/Release/Win32/glew32.lib
)

# ========================================
# | Geode build
# ========================================
if (NOT DEFINED ENV{GEODE_SDK})
    message(STATUS "Geode SDK not found, skipping Geode build")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")

    file(
        GLOB_RECURSE GEODE_SOURCE_FILES 
        ${CMAKE_CURRENT_LIST_DIR}/src/geode/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/geode/*.hpp
    )

    add_library(
        ${PROJECT_NAME}-Geode
        SHARED
        ${SHARED_SOURCE_FILES}
        ${GEODE_SOURCE_FILES}
    )

    add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

    target_link_libraries(
        ${PROJECT_NAME}-Geode
        ${PROJECT_NAME}
    )

    # Set "OPENHACK_GEODE" to 1
    target_compile_definitions(
        ${PROJECT_NAME}-Geode
        PRIVATE
        OPENHACK_GEODE=1
    )

    setup_geode_mod(${PROJECT_NAME}-Geode)
endif()

# ========================================
# | Standalone build
# ========================================
file(
    GLOB_RECURSE STANDALONE_SOURCE_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/src/standalone/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/standalone/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/standalone/*.def
)

add_library(
    ${PROJECT_NAME}-Standalone
    SHARED
    ${SHARED_SOURCE_FILES}
    ${STANDALONE_SOURCE_FILES}
)

add_subdirectory(libs/minhook)
add_subdirectory(libs/glfw)
add_subdirectory(libs/spdlog)

target_include_directories(
    ${PROJECT_NAME}-Standalone
    PRIVATE
    libs/glew/include
)

target_link_libraries(
    ${PROJECT_NAME}-Standalone
    ${PROJECT_NAME}
    minhook
    spdlog::spdlog
)

# Set "OPENHACK_STANDALONE" to 1
target_compile_definitions(
    ${PROJECT_NAME}-Standalone
    PRIVATE
    OPENHACK_STANDALONE=1
)

set_target_properties(
    ${PROJECT_NAME}-Standalone 
    PROPERTIES 
    OUTPUT_NAME "xinput9_1_0"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Create "openhack" directory in the output directory and copy everything from "resources" to it
add_custom_command(
    TARGET ${PROJECT_NAME}-Standalone
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/openhack"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/openhack"
)