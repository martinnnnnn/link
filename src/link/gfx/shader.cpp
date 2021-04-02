#include "shader.hpp"


#include <glm/gtc/type_ptr.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

//char* shader_src[3];
//shader_src[0] = "#version ...\n";
//shader_src[1] = ReadHeaderFile(....);
//shader_src[2] = ReadShaderSourceFile(....);
//glShaderSource(shader, 3, shader_src, NULL);


namespace link
{
    Shader::Shader()
        : id(0)
        , vertex_object(0)
        , fragment_object(0)
        , geometry_object(0)
    {
    }

    Shader::~Shader()
    {
        unload();
    }

    bool Shader::load(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
    {
        bool success = false;

        this->vertex_path = vertex_path;
        this->fragment_path = fragment_path;
        this->geometry_path = geometry_path;

        std::string vertex_code;
        std::string fragment_code;
        std::string geometry_code;

        if (!vertex_path.empty() && !File::exists(vertex_path))
        {
            fmt::print("[SHADER::open] Failed to open vertex shader with path : {}\n", vertex_path.c_str());
        }
        else
        {
            File::read(vertex_path, vertex_code);
        }

        if (!fragment_path.empty() && !File::exists(fragment_path))
        {
            fmt::print("[SHADER::open] Failed to open fragment shader with path : \n{}", fragment_path.c_str());
        }
        else
        {
            File::read(fragment_path, fragment_code);
        }

        if (!geometry_path.empty() && !File::exists(geometry_path))
        {
            fmt::print("[SHADER::open] Failed to open geometry shader with path : \n{}", geometry_path.c_str());
        }
        else
        {
            File::read(geometry_path, geometry_code);
        }

        if (vertex_code != "" && fragment_code != "")
        {
            //load(vertex_code.c_str(), fragment_code.c_str(), geometry_code != "" ? geometry_code.c_str() : nullptr);

            id = glCreateProgram();

            vertex_object = glCreateShader(GL_VERTEX_SHADER);
            gpu_load(vertex_object, vertex_code.c_str());

            fragment_object = glCreateShader(GL_FRAGMENT_SHADER);
            gpu_load(fragment_object, fragment_code.c_str());

            if (!geometry_code.empty())
            {
                geometry_object = glCreateShader(GL_GEOMETRY_SHADER);
                gpu_load(geometry_object, geometry_code.c_str());
            }

            glAttachShader(id, vertex_object);
            glAttachShader(id, fragment_object);
            if (!geometry_code.empty())
            {
                glAttachShader(id, geometry_object);
            }

            glLinkProgram(id);
            check_compile_errors(id, "PROGRAM");


            glDeleteShader(vertex_object);
            glDeleteShader(fragment_object);
            if (!geometry_code.empty())
            {
                glDeleteShader(geometry_object);
            }

            success = true;
        }

        return success;
    }

    bool Shader::load(const char* vertex_code, const char* fragment_code)
    {
        if (vertex_code != "" && fragment_code != "")
        {
            id = glCreateProgram();

            vertex_object = glCreateShader(GL_VERTEX_SHADER);
            gpu_load(vertex_object, vertex_code);

            fragment_object = glCreateShader(GL_FRAGMENT_SHADER);
            gpu_load(fragment_object, fragment_code);

            glAttachShader(id, vertex_object);
            glAttachShader(id, fragment_object);

            glLinkProgram(id);
            check_compile_errors(id, "PROGRAM");


            glDeleteShader(vertex_object);
            glDeleteShader(fragment_object);

            return true;
        }
        return false;
    }

    std::vector<Uniform> Shader::get_uniforms()
    {
        std::vector<Uniform> uniforms;

        GLint count;

        GLint size; // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)

        const GLsizei bufSize = 32; // maximum name length
        GLchar name[bufSize]; // variable name in GLSL
        GLsizei length; // name length

        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

        for (u32 i = 0; i < count; i++)
        {
            glGetActiveUniform(id, (GLuint)i, bufSize, &length, &size, &type, name);
            uniforms.emplace_back(i, type, name);
        }

        return uniforms;
        //GL_FLOAT_VEC3 GL_SAMPLER_2D GL_FLOAT_MAT4 GL_FLOAT
    }

    void Shader::bind_ub(const std::string& block_name, const BindingPoint binding_point)
    {
        const GLuint block_index = glGetUniformBlockIndex(id, block_name.c_str());

        if (block_index == GL_INVALID_INDEX)
        {
            fmt::print("WARNING :: Could not get index of uniform block {}\n", block_name);
            return;
        }

        glUniformBlockBinding(id, block_index, (GLuint)binding_point);
    }


    bool Shader::reload()
    {
        fmt::print("\n\nShader Reloading !!\n");
        unload();
        return load(vertex_path, fragment_path, geometry_path);
    }

    void Shader::gpu_load(GLuint obj, const char* code)
    {
        glShaderSource(obj, 1, &code, NULL);
        glCompileShader(obj);
        check_compile_errors(obj, "VERTEX");
    }

    void Shader::unload()
    {
        glDeleteProgram(id);
    }


    void Shader::use() const
    {
        glUseProgram(id);
    }

    u32 Shader::set(const std::string& _name, bool _value) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform1i(parameter_id, (int)_value);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, int _value) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform1i(parameter_id, _value);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, float _value) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform1f(parameter_id, _value);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, float _x, float _y) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform2f(parameter_id, _x, _y);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, float _x, float _y, float _z) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform3f(parameter_id, _x, _y, _z);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, float x, float y, float z, float w) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform4f(parameter_id, x, y, z, w);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, const glm::vec2& _vector) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform2f(parameter_id, _vector.x, _vector.y);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, const glm::vec3& _vector) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniform3f(parameter_id, _vector.x, _vector.y, _vector.z);
        return parameter_id;
    }

    u32 Shader::set(const std::string& name, const glm::vec4& v) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, name.c_str());
        glUniform4f(parameter_id, v.x, v.y, v.z, v.w);
        return parameter_id;
    }

    u32 Shader::set(const std::string& _name, const glm::mat4& _matrix) const
    {
        u32 parameter_id = (u32)glGetUniformLocation(id, _name.c_str());
        glUniformMatrix4fv(parameter_id, 1, GL_FALSE, glm::value_ptr(_matrix));
        return parameter_id;
    }

    void Shader::set(u32 _parameterID, bool _value) const
    {
        glUniform1i(_parameterID, (int)_value);
    }
    void Shader::set(u32 _parameterID, int _value) const
    {
        glUniform1i(_parameterID, _value);
    }
    void Shader::set(u32 _parameterID, float _x) const
    {
        glUniform1f(_parameterID, _x);
    }
    void Shader::set(u32 _parameterID, float _x, float _y) const
    {
        glUniform2f(_parameterID, _x, _y);
    }
    void Shader::set(u32 _parameterID, float _x, float _y, float _z) const
    {
        glUniform3f(_parameterID, _x, _y, _z);
    }
    void Shader::set(u32 _parameterID, float _x, float _y, float _z, float _w) const
    {
        glUniform4f(_parameterID, _x, _y, _z, _w);
    }
    void Shader::set(u32 _parameterID, const glm::vec2& _vector) const
    {
        glUniform2f(_parameterID, _vector.x, _vector.y);
    }
    void Shader::set(u32 _parameterID, const glm::vec3& _vector) const
    {
        glUniform3f(_parameterID, _vector.x, _vector.y, _vector.z);
    }
    void Shader::set(u32 _parameterID, const glm::vec4& v) const
    {
        glUniform4f(_parameterID, v.x, v.y, v.z, v.w);
    }
    void Shader::set(u32 _parameterID, const glm::mat4& _matrix) const
    {
        glUniformMatrix4fv(_parameterID, 1, GL_FALSE, glm::value_ptr(_matrix));
    }

    void Shader::check_compile_errors(unsigned int shader, const std::string& type)
    {
        fmt::print("Compiling shader {}... ", shader);
        int success;
        char infoLog[1024];

        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                fmt::print("\nERROR::SHADER_COMPILATION_ERROR of type: {}\n{}\n-------------------------------------------------------\n", type.c_str(), infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                fmt::print("\nERROR::PROGRAM_LINKING_ERROR of type: {}\n{}\n-------------------------------------------------------\n", type.c_str(), infoLog);
            }
        }
        if (success)
        {
            fmt::print("SUCCESS !\n");
        }
    }
}

//glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &count);
//printf("Active Attributes: %d\n", count);

//for (i = 0; i < count; i++)
//{
//    glGetActiveAttrib(id, (GLuint)i, bufSize, &length, &size, &type, name);

//    printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
//} 

/*

* glGetActiveAttrib
GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4,
GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_FLOAT_MAT2x3, GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, GL_FLOAT_MAT4x3,
GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4,
GL_UNSIGNED_INT, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4,
GL_DOUBLE, GL_DOUBLE_VEC2, GL_DOUBLE_VEC3, GL_DOUBLE_VEC4,
GL_DOUBLE_MAT2, GL_DOUBLE_MAT3, GL_DOUBLE_MAT4, GL_DOUBLE_MAT2x3, GL_DOUBLE_MAT2x4, GL_DOUBLE_MAT3x2, GL_DOUBLE_MAT3x4, GL_DOUBLE_MAT4x2, or GL_DOUBLE_MAT4x3

* glGetActiveUniform
GL_FLOAT 	                                    float
GL_FLOAT_VEC2 	                                vec2
GL_FLOAT_VEC3 	                                vec3
GL_FLOAT_VEC4 	                                vec4
GL_DOUBLE 	                                    double
GL_DOUBLE_VEC2 	                                dvec2
GL_DOUBLE_VEC3 	                                dvec3
GL_DOUBLE_VEC4 	                                dvec4
GL_INT 	                                        int
GL_INT_VEC2 	                                ivec2
GL_INT_VEC3 	                                ivec3
GL_INT_VEC4 	                                ivec4
GL_UNSIGNED_INT 	                            unsigned int
GL_UNSIGNED_INT_VEC2 	                        uvec2
GL_UNSIGNED_INT_VEC3 	                        uvec3
GL_UNSIGNED_INT_VEC4 	                        uvec4
GL_BOOL 	                                    bool
GL_BOOL_VEC2 	                                bvec2
GL_BOOL_VEC3 	                                bvec3
GL_BOOL_VEC4 	                                bvec4
GL_FLOAT_MAT2 	                                mat2
GL_FLOAT_MAT3 	                                mat3
GL_FLOAT_MAT4 	                                mat4
GL_FLOAT_MAT2x3 	                            mat2x3
GL_FLOAT_MAT2x4 	                            mat2x4
GL_FLOAT_MAT3x2 	                            mat3x2
GL_FLOAT_MAT3x4 	                            mat3x4
GL_FLOAT_MAT4x2 	                            mat4x2
GL_FLOAT_MAT4x3 	                            mat4x3
GL_DOUBLE_MAT2 	                                dmat2
GL_DOUBLE_MAT3 	                                dmat3
GL_DOUBLE_MAT4 	                                dmat4
GL_DOUBLE_MAT2x3 	                            dmat2x3
GL_DOUBLE_MAT2x4 	                            dmat2x4
GL_DOUBLE_MAT3x2 	                            dmat3x2
GL_DOUBLE_MAT3x4 	                            dmat3x4
GL_DOUBLE_MAT4x2 	                            dmat4x2
GL_DOUBLE_MAT4x3 	                            dmat4x3
GL_SAMPLER_1D 	                                sampler1D
GL_SAMPLER_2D 	                                sampler2D
GL_SAMPLER_3D 	                                sampler3D
GL_SAMPLER_CUBE 	                            samplerCube
GL_SAMPLER_1D_SHADOW 	                        sampler1DShadow
GL_SAMPLER_2D_SHADOW 	                        sampler2DShadow
GL_SAMPLER_1D_ARRAY 	                        sampler1DArray
GL_SAMPLER_2D_ARRAY 	                        sampler2DArray
GL_SAMPLER_1D_ARRAY_SHADOW 	                    sampler1DArrayShadow
GL_SAMPLER_2D_ARRAY_SHADOW 	                    sampler2DArrayShadow
GL_SAMPLER_2D_MULTISAMPLE 	                    sampler2DMS
GL_SAMPLER_2D_MULTISAMPLE_ARRAY 	            sampler2DMSArray
GL_SAMPLER_CUBE_SHADOW 	                        samplerCubeShadow
GL_SAMPLER_BUFFER 	                            samplerBuffer
GL_SAMPLER_2D_RECT 	                            sampler2DRect
GL_SAMPLER_2D_RECT_SHADOW 	                    sampler2DRectShadow
GL_INT_SAMPLER_1D 	                            isampler1D
GL_INT_SAMPLER_2D 	                            isampler2D
GL_INT_SAMPLER_3D 	                            isampler3D
GL_INT_SAMPLER_CUBE 	                        isamplerCube
GL_INT_SAMPLER_1D_ARRAY 	                    isampler1DArray
GL_INT_SAMPLER_2D_ARRAY 	                    isampler2DArray
GL_INT_SAMPLER_2D_MULTISAMPLE 	                isampler2DMS
GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 	        isampler2DMSArray
GL_INT_SAMPLER_BUFFER 	                        isamplerBuffer
GL_INT_SAMPLER_2D_RECT 	                        isampler2DRect
GL_UNSIGNED_INT_SAMPLER_1D 	                    usampler1D
GL_UNSIGNED_INT_SAMPLER_2D 	                    usampler2D
GL_UNSIGNED_INT_SAMPLER_3D 	                    usampler3D
GL_UNSIGNED_INT_SAMPLER_CUBE 	                usamplerCube
GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 	            usampler2DArray
GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 	            usampler2DArray
GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 	        usampler2DMS
GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 	usampler2DMSArray
GL_UNSIGNED_INT_SAMPLER_BUFFER 	                usamplerBuffer
GL_UNSIGNED_INT_SAMPLER_2D_RECT 	            usampler2DRect
GL_IMAGE_1D 	                                image1D
GL_IMAGE_2D 	                                image2D
GL_IMAGE_3D 	                                image3D
GL_IMAGE_2D_RECT 	                            image2DRect
GL_IMAGE_CUBE 	                                imageCube
GL_IMAGE_BUFFER 	                            imageBuffer
GL_IMAGE_1D_ARRAY 	                            image1DArray
GL_IMAGE_2D_ARRAY 	                            image2DArray
GL_IMAGE_2D_MULTISAMPLE 	                    image2DMS
GL_IMAGE_2D_MULTISAMPLE_ARRAY 	                image2DMSArray
GL_INT_IMAGE_1D 	                            iimage1D
GL_INT_IMAGE_2D 	                            iimage2D
GL_INT_IMAGE_3D 	                            iimage3D
GL_INT_IMAGE_2D_RECT 	                        iimage2DRect
GL_INT_IMAGE_CUBE 	                            iimageCube
GL_INT_IMAGE_BUFFER 	                        iimageBuffer
GL_INT_IMAGE_1D_ARRAY 	                        iimage1DArray
GL_INT_IMAGE_2D_ARRAY 	                        iimage2DArray
GL_INT_IMAGE_2D_MULTISAMPLE 	                iimage2DMS
GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 	            iimage2DMSArray
GL_UNSIGNED_INT_IMAGE_1D 	                    uimage1D
GL_UNSIGNED_INT_IMAGE_2D 	                    uimage2D
GL_UNSIGNED_INT_IMAGE_3D 	                    uimage3D
GL_UNSIGNED_INT_IMAGE_2D_RECT 	                uimage2DRect
GL_UNSIGNED_INT_IMAGE_CUBE 	                    uimageCube
GL_UNSIGNED_INT_IMAGE_BUFFER 	                uimageBuffer
GL_UNSIGNED_INT_IMAGE_1D_ARRAY 	                uimage1DArray
GL_UNSIGNED_INT_IMAGE_2D_ARRAY 	                uimage2DArray
GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 	        uimage2DMS
GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 	    uimage2DMSArray
GL_UNSIGNED_INT_ATOMIC_COUNTER 	                atomic_uint

*/