#pragma once

#include <assert.h>
#include <glm/glm.hpp>
#include <json.hpp>

#include "editor.hpp"
#include "e_string.hpp"
#include "link/types.hpp"
#include "link/gfx/texture_2d.hpp"
#include "link/serialization.hpp"

namespace link
{
    struct Shader;

    struct ETexture2D
    {
        ETexture2D(const std::string& name, const std::string& shader_name, TextureType type, bool single_value_default, const glm::vec3& default_value = {});

        void set_values(Shader* shader, i32 texture_unit);

        std::string shader_name;
        std::string name;
        std::string path;
        Texture2D* value;
        glm::vec3 default_value;
        TextureType type;
        bool single_value_default;

        void to_json(json& j);
        void from_json(const json& j);

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw();
#else
        inline bool debug_draw() { return false; }
#endif
    };
}

// 09 77 41 03 81

