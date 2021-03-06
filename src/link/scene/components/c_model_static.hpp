#pragma once

#include <string>
#include <vector>
#include <memory>

#include "link/scene/component.hpp"
#include "link/editor/editor.hpp"
#include "link/editor/e_string.hpp"

namespace link
{
    struct CMaterial_PBR;
    struct CMaterial_Phong;
    struct CMaterial;
    struct CLightSource;
    struct Mesh;


    struct CModel_Static : Component
    {
        CModel_Static(SceneObject* owner);
        ~CModel_Static();

        void init() override;
        void refresh_dependances() override;

        void load();
        void draw(const std::set<CLightSource*>& light_sources, u32 lights_count);

        CMaterial* material;
        std::vector<Mesh*> meshes;
        EString path;
        bool flip_uvs;

        void to_json(json& j) override;
        void from_json(const json& j) override;

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw() override;
#else
        inline bool debug_draw() { return false; }
#endif

        LINK_DECLARE_RTTI
    };
}