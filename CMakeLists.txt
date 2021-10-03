cmake_minimum_required(VERSION 3.16.3)


#### GENERAL CMAKE SETTINGS ####
project(link)
message(STATUS "project name : ${PROJECT_NAME}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /GR-")



set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION true)

#### SETTING PROJECT MAIN PATHS AND NAMES ####
set(LINK_PROJECT link)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${LINK_PROJECT})

set(LINK_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src")
set(LINK_EXE_PATH "${CMAKE_SOURCE_DIR}/bin/${LINK_PROJECT}")

set(FMT_LIB fmt_lib)
set(FMT_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/external/fmt/include/")
set(FMT_LIB_PATH "${CMAKE_SOURCE_DIR}/bin/${FMT_LIB}")

set(IMGUI_LIB imgui_lib)
set(IMGUI_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/external/imgui-docking-732cd83/" "${CMAKE_SOURCE_DIR}/external/imgui-docking-732cd83/misc/cpp/")
set(IMGUI_LIB_PATH "${CMAKE_SOURCE_DIR}/bin/${IMGUI_LIB}")

set(BULLET_LIBS_NAMES
    # Bullet2FileLoader
    # Bullet3Collision
    # Bullet3Common
    # Bullet3Dynamics
    # Bullet3Geometry
    # Bullet3OpenCL_clew
    # BulletCollision
    # BulletDynamics
    # BulletFileLoader
    # BulletInverseDynamics
    # BulletInverseDynamicsUtils
    # BulletRobotics
    # BulletRoboticsGUI
    # BulletSoftBody
    # BulletWorldImporter
    # BulletXmlWorldImporter
    # BussIK
    # clsocket
    # ConvexDecomposition
    BulletDynamics_Debug BulletCollision_Debug LinearMath_Debug
    )
set(BULLET_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/external/bullet/include/")
# set(BULLET_LIB_PATH "${CMAKE_SOURCE_DIR}/external/bullet/libs/Release")
set(BULLET_LIB_PATH "${CMAKE_SOURCE_DIR}/external/bullet/libs/Debug")



link_directories(
    ${CMAKE_SOURCE_DIR}/external/glew-2.1.0/lib/Release/x64/
    ${CMAKE_SOURCE_DIR}/external/opengl/
    ${CMAKE_SOURCE_DIR}/external/sdl/libs/
    ${FMT_LIB_PATH}
    ${IMGUI_LIB_PATH}
    ${BULLET_LIB_PATH}
    ${CMAKE_SOURCE_DIR}/external/assimp-5.0.1/BINARIES/x64/code/Release)

#### GROUP CREATION ####
macro(GroupSources root_dir curdir)
    file(GLOB children RELATIVE ${root_dir}/${curdir}
            ${root_dir}/${curdir}/*)

    foreach(child ${children})
        if(IS_DIRECTORY ${root_dir}/${curdir}/${child})
            GroupSources(${root_dir} ${curdir}/${child})
        else()
            string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${curdir}/${child})

        endif()
    endforeach()
endmacro()

#### ADDING SOURCE ####
add_subdirectory(external)
add_subdirectory(src)