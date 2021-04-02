#pragma once

#include <glm/glm.hpp>

#include "link/input.hpp"

namespace link
{
    template<typename i32 Size>
    struct VolumeSize
    {
        //VolumeSize(i32 size)
        //    : value(Size)
        //    , squared(Size * Size)
        //    , cubed(Size * Size * Size)
        //{}

        static constexpr i32 value = Size;
        static constexpr i32 squared = Size * Size;
        static constexpr i32 cubed = Size * Size * Size;

        //const i32 value;
        //const i32 squared;
        //const i32 cubed;

        static inline i32          get_offset(const i32 x, const i32 y, const i32 z) { return x + y * value + z * squared; }
        static inline i32          get_offset(const glm::ivec3& position) { return get_offset(position.x, position.y, position.z); }
        static inline glm::ivec3   get_position(i32 offset)
        {
            const i32 z = offset / squared;
            const i32 y = (offset - z * squared) / value;
            const i32 x = (offset - z * squared - y * value);

            return  glm::ivec3{ x, y, z };
        }
    };
}


