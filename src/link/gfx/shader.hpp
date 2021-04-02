#pragma once


#include <GL/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <glm/glm.hpp>
#include <vector>

#include "ubo.hpp"
#include "link/types.hpp"
#include "link/file.hpp"

namespace link
{
    struct Uniform
    {
        Uniform(u32 id, GLenum type, const char* name)
            : id(id)
            , type(type)
            , name(name) {}

        u32 id;
        GLenum type;
        std::string name;
    };

    struct Shader
    {
        GLuint id;

        Shader();
        ~Shader();
        void use() const;
        bool load(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path = "");
        bool load(const char* vertex_code, const char* fragment_code);

        bool reload();
        void unload();

        u32 set(const std::string& _name, bool _value) const;
        u32 set(const std::string& _name, int _value) const;
        u32 set(const std::string& _name, float _x) const;
        u32 set(const std::string& _name, float _x, float _y) const;
        u32 set(const std::string& _name, float _x, float _y, float _z) const;
        u32 set(const std::string& _name, float _x, float _y, float _z, float _w) const;
        u32 set(const std::string& _name, const glm::vec2& _vector) const;
        u32 set(const std::string& _name, const glm::vec3& _vector) const;
        u32 set(const std::string& _name, const glm::vec4& _vector) const;
        u32 set(const std::string& _name, const glm::mat4& _matrix) const;

        void set(u32 _parameterID, bool _value) const;
        void set(u32 _parameterID, int _value) const;
        void set(u32 _parameterID, float _x) const;
        void set(u32 _parameterID, float _x, float _y) const;
        void set(u32 _parameterID, float _x, float _y, float _z) const;
        void set(u32 _parameterID, float _x, float _y, float _z, float _w) const;
        void set(u32 _parameterID, const glm::vec2& _vector) const;
        void set(u32 _parameterID, const glm::vec3& _vector) const;
        void set(u32 _parameterID, const glm::vec4& _vector) const;
        void set(u32 _parameterID, const glm::mat4& _matrix) const;

        std::vector<Uniform> get_uniforms();
        //void bind_ub(const std::string& block_name, const GLuint binding_point);
        void bind_ub(const std::string& block_name, const BindingPoint binding_point);

        std::string vertex_path;
        std::string fragment_path;
        std::string geometry_path;

    private:

        void check_compile_errors(unsigned int _shader, const std::string& _type);
        void gpu_load(GLuint obj, const char* code);

        GLuint geometry_object;
        GLuint vertex_object;
        GLuint fragment_object;

    };
}