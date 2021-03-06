#include "c_material_pbr.hpp"

#include <fmt/ostream.h>
#include <imgui.h>

#include "link/gfx/shader.hpp"
#include "link/gfx/ubo.hpp"
#include "link/gfx/renderer.hpp"
#include "link/gfx/camera.hpp"
#include "link/gfx/texture_2d.hpp"
#include "link/timer.hpp"
#include "link/editor/e_string.hpp"
#include "link/data_system.hpp"
#include "link/timer.hpp"
#include "c_transform.hpp"

namespace link
{
    LINK_IMPLEMENT_RTTI(link, CMaterial_PBR, CMaterial);

    CMaterial_PBR::CMaterial_PBR(SceneObject* owner)
        : CMaterial(owner, Type::Material_PBR)
        , albedo("Albedo Map", "albedo_tex", TextureType::PBR_ALBEDO, false, { 0.5f, 0.5f, 0.5f })
        , metallic("Metallic Map", "metallic_tex", TextureType::PBR_METALLIC, true)
        , roughness("Roughness Map", "roughness_tex", TextureType::PBR_ROUGHNESS, true)
        , ao("Ambient Occlusion Map", "ao_tex", TextureType::PBR_AMBIENT_OCCLUSION, true, { 1.0f, 1.0f,1.0f })
        , normal("Normal Map", "normal_tex", TextureType::PBR_NORMAL, false)
    {}

    void CMaterial_PBR::update()
    {
        transform->rotation.y += 0.1f * LINK_TIME->timer.dt_s;
    }


    void CMaterial_PBR::to_json(json& j)
    {
        CMaterial::to_json(j);
        albedo.to_json(j["albedo"]);
        metallic.to_json(j["metallic"]);
        roughness.to_json(j["roughness"]);
        ao.to_json(j["ao"]);
        normal.to_json(j["normal"]);
    }

    void CMaterial_PBR::from_json(const json& j)
    {
        CMaterial::from_json(j);
        albedo.from_json(j["albedo"]);
        metallic.from_json(j["metallic"]);
        roughness.from_json(j["roughness"]);
        ao.from_json(j["ao"]);
        normal.from_json(j["normal"]);
    }

    void CMaterial_PBR::set_shader_consts()
    {
        shader->use();
        shader->bind_ub("Camera", BindingPoint::CAMERA);
        shader->bind_ub("Lights", BindingPoint::LIGHT_SOURCES);
        shader->set("albedo_tex.value", 0);
        shader->set("normal_tex.value", 1);
        shader->set("metallic_tex.value", 2);
        shader->set("roughness_tex.value", 3);
        shader->set("ao_tex.value", 4);
    }

    void CMaterial_PBR::draw(u32 lights_count)
    {
        Camera* main_camera = LINK_RENDERER->main_camera;

        shader->use();
        shader->set("model", transform ? transform->get_matrix() : glm::mat4(1));
        shader->set("lights_count", (int)lights_count);

        albedo.set_values(shader.get(), GL_TEXTURE0);
        normal.set_values(shader.get(), GL_TEXTURE1);
        metallic.set_values(shader.get(), GL_TEXTURE2);
        roughness.set_values(shader.get(), GL_TEXTURE3);
        ao.set_values(shader.get(), GL_TEXTURE4);
    }

    void CMaterial_PBR::set_texture(Texture2D* texture)
    {
        switch (texture->type)
        {
        case TextureType::PBR_ALBEDO:
            albedo.value = texture;
            albedo.path = texture->file_path;
            break;
        case TextureType::PBR_NORMAL:
            normal.value = texture;
            normal.path = texture->file_path;
            break;
        case TextureType::PBR_EMISSIVE:
            break;
        case TextureType::PBR_METALLIC:
            metallic.value = texture;
            metallic.path = texture->file_path;
            break;
        case TextureType::PBR_ROUGHNESS:
            roughness.value = texture;
            roughness.path = texture->file_path;
            break;
        case TextureType::PBR_AMBIENT_OCCLUSION:
            ao.value = texture;
            ao.path = texture->file_path;
            break;
        }
    }


#ifdef LINK_EDITOR_ENABLED
    bool CMaterial_PBR::debug_draw()
    {
        if (!Component::debug_draw())
            return false;

        if (ImGui::Button("Reload Shader"))
        {
            shader->reload();
            set_shader_consts();
        }

        albedo.debug_draw();
        normal.debug_draw();
        metallic.debug_draw();
        roughness.debug_draw();
        ao.debug_draw();

        return true;
    }
#endif
}

