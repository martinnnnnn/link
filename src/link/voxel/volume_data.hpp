#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "link/types.hpp"
#include "volume_size.hpp"
#include "volume_sample.hpp"
#include "volume_chunk.hpp"


namespace link
{

    template<typename i32 Size, typename i32 ChunkSize>
    struct VolumeData
    {
        using VolumeChunkMap = std::array<std::unique_ptr<VolumeChunk<ChunkSize>>, VolumeSize<Size>::cubed>;

        VolumeChunkMap chunks;

        VolumeData()
        {
            for (u64 x = 0; x < VolumeSize<Size>::value; ++x)
            {
                for (u64 y = 0; y < VolumeSize<Size>::value; ++y)
                {
                    for (u64 z = 0; z < VolumeSize<Size>::value; ++z)
                    {
                        chunks[x + y * VolumeSize<Size>::value + z * VolumeSize<Size>::squared] = std::make_unique<VolumeChunk<ChunkSize>>(glm::ivec3{ x, y, z });
                    }
                }
            }
        }

        ~VolumeData() = default;

        inline VolumeChunk<ChunkSize>* chunk(i32 x, i32 y, i32 z)
        {
            return chunks[x + y * VolumeSize<ChunkSize>::value + z * VolumeSize<ChunkSize>::squared].get();
        }

        inline Sample* sample(i32 x, i32 y, i32 z)
        {
            static Sample* default_sample = new Sample(0);
            Sample* sample = default_sample;

            const glm::ivec3 chunk_position { x / ChunkSize, y / ChunkSize, z / ChunkSize };

            i32 chunk_index = chunk_position.x + (chunk_position.y * VolumeSize<ChunkSize>::value) + (chunk_position.z * VolumeSize<ChunkSize>::squared);
            if (chunk_index >= 0 && chunk_index < VolumeSize<Size>::cubed)
            {
                std::unique_ptr<VolumeChunk<ChunkSize>>& chunk = chunks[chunk_index];

                // computing offset to the right sample in chunk
                const i32 xoff = (x - chunk->position.x * VolumeSize<ChunkSize>::value);
                const i32 yoff = (y - chunk->position.y * VolumeSize<ChunkSize>::value) * VolumeSize<ChunkSize>::value;
                const i32 zoff = (z - chunk->position.z * VolumeSize<ChunkSize>::value) * VolumeSize<ChunkSize>::squared;

                const i32 offset = xoff + yoff + zoff;

                if (offset >= 0 && offset < VolumeSize<ChunkSize>::cubed)
                {
                    sample = &(chunk->samples[offset]);
                }
            }

            return sample;
        }

        inline Sample* sample(glm::ivec3 offset) { return sample(offset.x, offset.y, offset.z); }


        // chunk loading
        // mesh generation
        // reusing mesh VBO
        // modify
        // intersection check with ray
        // load from file
    };

    constexpr u32 DEFAULT_CHUNK_SIZE = 32;
    constexpr u32 DEFAULT_DATA_SIZE = 4;
    using VolumeChunk32 = VolumeChunk<DEFAULT_CHUNK_SIZE>;
    using VolumeData32 = VolumeData<DEFAULT_DATA_SIZE, DEFAULT_CHUNK_SIZE>;
    using VolumeSize32 = VolumeSize<DEFAULT_CHUNK_SIZE>;
}


