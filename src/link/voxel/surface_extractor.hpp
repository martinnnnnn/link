#pragma once


#include "volume_data.hpp"

namespace link
{
    namespace SurfaceExtractor
    {
        void transvoxel(VolumeData32* data, VolumeChunk32* chunk);
    };
}