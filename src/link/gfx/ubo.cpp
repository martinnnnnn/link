#include "ubo.hpp"


namespace link
{

    UBO::UBO()
        : id(0)
        , size(0)
    {

    }

    UBO::~UBO()
    {
        clear();
    }

    void UBO::create(const u32 s, const BindingPoint b_point)
    {
        size = s;

        glGenBuffers(1, &id);

        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);

        glBindBufferRange(GL_UNIFORM_BUFFER, (GLuint)b_point, id, 0, size);
    }

    void UBO::bind_ubo() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void UBO::set_data(const size_t offset, const size_t size, const void* data)
    {
        bind_ubo();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

    void UBO::bind_range(const BindingPoint b_point, const u32 offset, const u32 size)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, (GLuint)b_point, id, offset, size);
    }

    void UBO::clear()
    {
        glDeleteBuffers(1, &id);
    }
}
