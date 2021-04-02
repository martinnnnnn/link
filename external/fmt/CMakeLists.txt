 
add_library(${FMT_LIB} STATIC src/format.cc src/os.cc)

set_target_properties(${FMT_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${FMT_LIB_PATH})
target_include_directories(${FMT_LIB} PUBLIC
    ${FMT_INCLUDE_PATH})