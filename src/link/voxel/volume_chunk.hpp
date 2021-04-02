#pragma once

#include <glm/glm.hpp>
#include <array>
#include <memory>

#include "link/types.hpp"
#include "volume_sample.hpp"
#include "volume_size.hpp"
#include "link/gfx/mesh.hpp"

namespace link
{
    template<typename i32 Size>
    struct VolumeChunk
    {
        VolumeChunk(const glm::ivec3& position);

        void draw()
        {
            mesh->draw();
        }

        inline Sample* get(u32 offset);
        inline Sample* get(u32 x, u32 y, u32 z);
        inline Sample* get(const glm::uvec3& position);

        glm::ivec3 position;
        std::array<Sample, VolumeSize<Size>::cubed> samples;
        std::unique_ptr<Mesh> mesh;
    };


    template<typename i32 Size>
    VolumeChunk<Size>::VolumeChunk(const glm::ivec3& position)
        : position(position)
    {
        for (i32 x = 0; x < VolumeSize<Size>::value; x++)
        {
            for (i32 y = 0; y < VolumeSize<Size>::value; y++)
            {
                for (i32 z = 0; z < VolumeSize<Size>::value; z++)
                {
                    if (x == 0 || y == 0 || z == 0 || x == VolumeSize<Size>::value - 1 || y == VolumeSize<Size>::value - 1 || z == VolumeSize<Size>::value - 1)
                    {
                        get(x, y, z)->value = 0;
                    }
                    else
                    {
                        get(x, y, z)->value = 255;
                    }
                }
            }
        }
    }


    template<typename i32 Size>
    Sample* VolumeChunk<Size>::get(u32 offset)
    {
        return &(samples[offset]);
    }

    template<typename i32 Size>
    Sample* VolumeChunk<Size>::get(u32 x, u32 y, u32 z)
    { 
        return get(x + y * VolumeSize<Size>::value + z * VolumeSize<Size>::squared);
    }

    template<typename i32 Size>
    Sample* VolumeChunk<Size>::get(const glm::uvec3& position)
    { 
        return get(position.x, position.y, position.z);
    }
}


