# External libraries setup
# -------------------------

add_library(external_libs INTERFACE)

# ImGui
add_library(
    imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)

if (WIN32)
    # Windows specific files
    target_sources(
        imgui PRIVATE
        imgui/backends/imgui_impl_win32.cpp
        imgui/backends/imgui_impl_opengl3.cpp
    )
endif()

target_include_directories(
    imgui PUBLIC
    imgui
    imgui/backends
)

# Other libraries
add_subdirectory(json)
add_subdirectory(zephyrus)
add_subdirectory(gd.hpp)
add_subdirectory(sinaps)

# Link all external libraries
target_link_libraries(
    external_libs INTERFACE
    imgui
    gd.hpp
    nlohmann_json::nlohmann_json
    Zephyrus
    sinaps
)

if (WIN32)
    # GLFW
    add_subdirectory(glfw)
    add_subdirectory(discord-rpc)

    target_link_libraries(
        external_libs INTERFACE
        glfw
        discord-rpc
    )
endif()