#include "c_model_static.hpp"

#include <imgui.h>

#include "link/gfx/renderer.hpp"
#include "link/gfx/mesh.hpp"
#include "c_material.hpp"
#include "c_material_pbr.hpp"
#include "c_material_phong.hpp"
#include "c_light_source.hpp"
#include "link/data_system.hpp"
#include "link/editor/editor.hpp"

namespace link
{
    namespace
    {
    }

    LINK_IMPLEMENT_RTTI(link, CModel_Static, Component);

    CModel_Static::CModel_Static(SceneObject* owner)
        : Component(owner, Type::Model_Static)
        , material(nullptr)
        , path("Model")
        , flip_uvs(false)
    {
        LINK_RENDERER->subscribe(this);
    }

    CModel_Static::~CModel_Static()
    {
        LINK_RENDERER->unsubscribe(this);
    }

    void CModel_Static::to_json(json& j)
    {
        Component::to_json(j);
        path.to_json(j["path"], true);
        j["flip_uvs"] = flip_uvs;
    }

    void CModel_Static::from_json(const json& j)
    {
        Component::from_json(j);
        path.from_json(j["path"], true);
        flip_uvs = j["flip_uvs"];

        if (!path.value.empty())
        {
            load();
        }
    }

    void CModel_Static::init()
    {
        refresh_dependances();
    }

    void CModel_Static::refresh_dependances()
    {
        material = get_component<CMaterial>();
        assert(material);
    }

    void CModel_Static::load()
    {
        DataSystem::AssimpModel* model = LINK_DATA_SYSTEM->load_model_assimp(path.value, flip_uvs);
        if (model)
        {
            meshes.clear();
            for (auto& model_mesh : model->meshes)
            {
                meshes.emplace_back(model_mesh.get());
            }

            for (Texture2D* texture : model->textures)
            {
                if (material)
                {
                    material->set_texture(texture);
                }
            }
        }
        else
        {
            fmt::print("Failed to load model\n");
            if (meshes.empty())
            {
                path.set("");
            }
        }
    }

    void CModel_Static::draw(const std::set<CLightSource*>& light_sources, u32 lights_count)
    {

        material->draw(lights_count);

        if (meshes.empty())
        {
            Mesh* sphere = LINK_DATA_SYSTEM->get_sphere();
            sphere->draw();
            //Mesh* cube = LINK_DATA_SYSTEM->get_cube();
            //cube->draw();
        }
        else
        {
            for (Mesh* mesh : meshes)
            {
                mesh->draw();
            }
        }
    }

#ifdef LINK_EDITOR_ENABLED
    bool CModel_Static::debug_draw()
    {
        if (!Component::debug_draw())
            return false;

        bool previous_flip_uvs = flip_uvs;
        ImGui::Checkbox("Flip UVs", &flip_uvs);

        path.debug_draw();

        if (EUtils::Button("CModel_Static", "Load"))
        {
            load();
        }

        //bool has_changed = path.debug_draw() || previous_flip_uvs != flip_uvs;
        //if (has_changed)
        //{
        //    DataSystem::AssimpModel* model = LINK_DATA_SYSTEM->load_model_assimp(path.value, flip_uvs);
        //    if (model)
        //    {
        //        meshes.insert(meshes.end(), model->meshes.begin(), model->meshes.end());

        //        for (Texture2D* texture : model->textures)
        //        {
        //            material->set_texture(texture);
        //        }
        //    }
        //    else
        //    {
        //        path.set("");
        //    }
        //}
        return true;
    }
#endif
}

