add_library(libcurl INTERFACE)

target_include_directories(libcurl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_libraries(libcurl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/libcurl.lib)
else()
    target_link_libraries(libcurl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/libcurl.lib)
endif()