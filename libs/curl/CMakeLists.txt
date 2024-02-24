add_library(libcurl INTERFACE)

target_include_directories(libcurl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(libcurl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libcurl.lib)