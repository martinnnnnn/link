
# project(tiny_csg)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /MP /GR-")

add_library(${CSG_LIB}
    src/csg.cpp
    src/query_point.cpp
    src/query_box.cpp
    src/query_ray.cpp
    src/query_frustum.cpp
    src/rebuild.cpp
)

target_include_directories(${CSG_LIB} PUBLIC ${CSG_INCLUDE_PATH})
target_include_directories(${CSG_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/external/glm-0.9.9.8/)

set_target_properties(${CSG_LIB} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CSG_LIB_PATH}
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")