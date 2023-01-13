file(GLOB_RECURSE LINK_SOURCE_FILES "*.h" "*.hpp" "*.cpp" "*.inl")


add_executable(${LINK_PROJECT} ${LINK_SOURCE_FILES} ${CMAKE_SOURCE_DIR})

target_link_libraries(${LINK_PROJECT}
    SDL2
    SDL2main
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    glew32
    OpenGL32
    ${FMT_LIB}
    ${CSG_LIB}
    ${IMGUI_LIB}
    ${BULLET_LIBS_NAMES}
    assimp-vc142-mt
)

target_include_directories(${LINK_PROJECT} PUBLIC
    ${LINK_INCLUDE_PATH}
    ${FMT_INCLUDE_PATH}
    ${CSG_INCLUDE_PATH}
    ${IMGUI_INCLUDE_PATH}
    ${BULLET_INCLUDE_PATH}
    ${CMAKE_SOURCE_DIR}/external/glew-2.1.0/include/
    ${CMAKE_SOURCE_DIR}/external/sdl/headers/SDL2-2.0.12/
    ${CMAKE_SOURCE_DIR}/external/sdl/headers/SDL2_image-2.0.5/
    ${CMAKE_SOURCE_DIR}/external/sdl/headers/SDL2_ttf-2.0.15/
    ${CMAKE_SOURCE_DIR}/external/sdl/headers/SDL2_mixer-2.0.4/
    ${CMAKE_SOURCE_DIR}/external/glm-0.9.9.8/
    ${CMAKE_SOURCE_DIR}/external/stb/
    ${CMAKE_SOURCE_DIR}/external/assimp-5.0.1/BINARIES/x64/include/
    ${CMAKE_SOURCE_DIR}/external/assimp-5.0.1/include/
    ${CMAKE_SOURCE_DIR}/external/modern-json/
    # ${CMAKE_SOURCE_DIR}/external/headers/asio-1.18.0/
    )

target_compile_definitions(${LINK_PROJECT} PRIVATE -DBT_USE_DOUBLE_PRECISION)

set_target_properties(${LINK_PROJECT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${LINK_EXE_PATH}
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions(${LINK_PROJECT} PUBLIC LINK_PROJ_DATA_ROOT="${CMAKE_SOURCE_DIR}/data/")

GroupSources(${CMAKE_CURRENT_SOURCE_DIR} link)

add_custom_command(TARGET ${LINK_PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/exe_root/$<CONFIGURATION>
        ${LINK_EXE_PATH}/$<CONFIGURATION>)
    