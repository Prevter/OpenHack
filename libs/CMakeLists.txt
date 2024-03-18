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
    imgui/backends/imgui_impl_win32.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(
    imgui PUBLIC
    imgui
    imgui/backends
)

target_link_libraries(
    imgui PUBLIC
    opengl32
)

# Other libraries
add_subdirectory(gd.hpp)
add_subdirectory(json)
add_subdirectory(glfw)
add_subdirectory(discord-rpc)
add_subdirectory(zephyrus)

# Link all external libraries
target_link_libraries(
    external_libs INTERFACE
    imgui
    glfw
    gd.hpp
    nlohmann_json::nlohmann_json
    discord-rpc
    Zephyrus
)