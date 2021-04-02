#pragma once


#include <glm/glm.hpp>

#include "link/types.hpp"


namespace link
{
    struct FrameBuffer
    {
        u32 fbo;
        u32 texture;
        u32 rbo_depth_stencil;
        glm::vec2 size;

        FrameBuffer();

        void init(const glm::vec2& buffer_size);
        void bind();
        void unbind();
        void clear();
    };
}