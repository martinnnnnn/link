#include "e_uniform.hpp"

#include "link/serialization.hpp"

#include <imgui.h>

namespace link
{
    void EUniform::to_json(json& j)
    {
        j["type"] = uniform.type;
        j["name"] = uniform.name;

        switch (uniform.type)
        {
        case GL_FLOAT:
        {
            j["float32"] = float32;
            break;
        }
        case GL_FLOAT_VEC3:
        {
            JsonUtils::to_json(j, vec3, "vec3");
            break;
        }
        case GL_SAMPLER_2D:
        {
            break;
        }
        case GL_FLOAT_MAT4:
        {
            break;
        }
        }
    }

    void EUniform::from_json(const json& j)
    {
        uniform.type = j["type"];
        uniform.name = j["name"];

        switch (uniform.type)
        {
        case GL_FLOAT:
        {
            float32 = j["float32"];
            break;
        }
        case GL_FLOAT_VEC3:
        {
            JsonUtils::from_json(j, vec3, "vec3");
            break;
        }
        case GL_SAMPLER_2D:
        {
            break;
        }
        case GL_FLOAT_MAT4:
        {
            break;
        }
        }
    }


#ifdef LINK_EDITOR_ENABLED
    bool EUniform::debug_draw()
    {
        // GL_SAMPLER_2D
        switch (uniform.type)
        {
        case GL_FLOAT:
        {
            ImGui::DragFloat(uniform.name.c_str(), (float*)&float32, 0.01f, 0.0f, 1.0f);
            break;
        }
        case GL_FLOAT_VEC3:
        {
            ImGui::ColorEdit3(uniform.name.c_str(), (float*)&vec3);
            break;
        }
        case GL_SAMPLER_2D:
        {
            assert(false && "not supported");
            break;
        }
        case GL_FLOAT_MAT4:
        {
            //assert(false && "not supported");
            break;
        }
        }
        return true;
    }
#endif
}

