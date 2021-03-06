#pragma once

#include "link/scene/component.hpp"
#include "link/gfx/light_sources.hpp"
#include "link/editor/editor.hpp"
#include "link/gfx/shader.hpp"
#include "link/gfx/ubo.hpp"

namespace link
{
    struct CTransform;
    struct CMaterial;

    struct CLightSource : Component
    {

        CLightSource(SceneObject* owner);
        virtual ~CLightSource();

        void init() override;
        void update() override;
        void refresh_dependances() override;

        void set_values(UBO& ubo, u32& offset, u32& count);

        void to_json(json& j) override;
        void from_json(const json& j) override;

        CTransform* transform;
        CMaterial* material;
        LightSource light;

        glm::vec3 color;
        bool is_on;
        f32 cutoff;
        f32 outer_cutoff;
        i32 selected_attenuation;

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw() override;
        void debug_update() override;
#else
        inline bool debug_draw() { return false; }
#endif

        LINK_DECLARE_RTTI
    };
}
