#include "framebuffer.hpp"

#include <GL/glew.h>
#include <gl/GL.h>

#include <fmt/ostream.h>


namespace link
{
    FrameBuffer::FrameBuffer()
        : fbo(0)
        , texture(0)
        , rbo_depth_stencil(0)
        , size(0, 0) {}

    void FrameBuffer::init(const glm::vec2& buffer_size)
    {
        if (fbo && size != buffer_size)
        {
            clear();
        }

        size = buffer_size;

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // attaching color texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // attaching depth and stencil renderbuffer as we only need to write to it
        glGenRenderbuffers(1, &rbo_depth_stencil);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_stencil);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_depth_stencil);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fmt::print("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, size.x, size.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void FrameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::clear()
    {
        if (fbo)
        {
            // TODO : find out why glDeleteFramebuffers crashes
            glDeleteFramebuffers(1, &fbo);
            glDeleteTextures(1, &texture);
            glDeleteRenderbuffers(1, &rbo_depth_stencil);
            texture = 0;
            rbo_depth_stencil = 0;
        }
    }
}