#pragma once

#include <glm/glm.hpp>
#include <json.hpp>

#include "link/types.hpp"
#include "editor.hpp"

namespace link
{
    struct Shader;
    struct CTransform;
    struct Texture2D;

    struct EUniform
    {
        EUniform(const Uniform& uniform)
            : uniform(uniform)
            , vec3{}
            , texture_2d(nullptr)
            , mat4(glm::mat4(1))
            , float32(0.0f)
        {
        }

        Uniform uniform;
        union
        {
            glm::vec3 vec3;
            Texture2D* texture_2d;
            glm::mat4 mat4;
            f32 float32;
        };

        using json = nlohmann::json;
        void to_json(json& j);
        void from_json(const json& j);

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw();
#else
        inline bool debug_draw() { return false; }
#endif
    };
}

