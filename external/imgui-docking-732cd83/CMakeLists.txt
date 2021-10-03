 
add_library(${IMGUI_LIB} STATIC
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    misc/cpp/imgui_stdlib.cpp
    backends/imgui_impl_sdl.cpp
    backends/imgui_impl_opengl3.cpp)

set(LIBS
    SDL2
    glew32
    OpenGL32)

target_link_libraries(${IMGUI_LIB} ${LIBS})

target_include_directories(${IMGUI_LIB} PUBLIC 
    ${CMAKE_SOURCE_DIR}/external/sdl/headers/SDL2-2.0.12/
    ${CMAKE_SOURCE_DIR}/external/glew-2.1.0/include/)

set_target_properties(${IMGUI_LIB} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${IMGUI_LIB_PATH}
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${IMGUI_LIB} PUBLIC
    ${IMGUI_INCLUDE_PATH})