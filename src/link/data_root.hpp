#pragma once

//#define USE_PROJ_PATH

#include <filesystem>
namespace fs = std::filesystem;

namespace link
{
    namespace root
    {
#ifdef USE_PROJ_PATH
    #define LINK_DATA_ROOT LINK_PROJ_DATA_ROOT
#else
    #define LINK_DATA_ROOT fs::current_path().string() + "/data/"
#endif
    }
}
