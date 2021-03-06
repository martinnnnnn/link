#pragma once

#include <GL/glew.h>
#include <gl/GL.h>

#include "link/types.hpp"

namespace link
{
    enum class BindingPoint
    {
        CAMERA = 0,
        LIGHT_SOURCES = 1
        //LIGHT_POINTS = 1,
        //LIGHT_SPOTS = 2,
        //PBR_LIGHTS = 2,
    };

    struct UBO
    {
        UBO();
        ~UBO();

        void create(const u32 s, const BindingPoint b_point);
        void bind_ubo() const;
        void set_data(const size_t offset, const size_t size, const void* data);
        void bind_range(const BindingPoint b_point, const u32 offset, const u32 size);
        void clear();

        GLuint id;
        u32 size;
    };
}
