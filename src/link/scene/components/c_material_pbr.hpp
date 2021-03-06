#pragma once

#include <string>
#include <memory>
#include <json.hpp>
#include <glm/glm.hpp>

#include "c_material.hpp"
#include "link/types.hpp"
#include "link/editor/editor.hpp"
#include "link/editor/e_texture_2d.hpp"
#include "link/editor/e_string.hpp"

namespace link
{
    struct Shader;
    struct CTransform;
    struct Texture2D;

    struct CMaterial_PBR : public CMaterial
    {
        CMaterial_PBR(SceneObject* owner);

        void update() override;

        void draw(u32 lights_count) override;
        void set_shader_consts() override;
        void set_texture(Texture2D* texture) override;

        void to_json(json& j) override;
        void from_json(const json& j) override;

        ETexture2D albedo;
        ETexture2D metallic;
        ETexture2D roughness;
        ETexture2D ao;
        ETexture2D normal;

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw() override;
#else
        inline bool debug_draw() { return false; }
#endif

        LINK_DECLARE_RTTI
    };
}

