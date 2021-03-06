#include "c_material_phong.hpp"

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
#include "c_transform.hpp"

namespace link
{
    LINK_IMPLEMENT_RTTI(link, CMaterial_Phong, CMaterial);

    CMaterial_Phong::CMaterial_Phong(SceneObject* owner)
        : CMaterial(owner, Type::Material_Phong)
        , diffuse("Diffuse Map", "diffuse_tex", TextureType::DIFFUSE, false, { 0.5f, 0.5f, 0.5f })
        , normal("Normal Map", "normal_tex", TextureType::NORMAL, false)
        , specular("Specular Map", "specular_tex", TextureType::SPECULAR , false, { 0.5f, 0.5f, 0.5f })
        , ambient("Ambient Map", "ambient_tex", TextureType::AMBIENT, false, { 0.5f, 0.5f, 0.5f })
        , shininess("Ambient Map", "ambient_tex", TextureType::AMBIENT, true, { 32.0f, 32.0f, 32.0f })
    {}

    void CMaterial_Phong::to_json(json& j)
    {
        CMaterial::to_json(j);
        diffuse.to_json(j["diffuse"]);
        normal.to_json(j["normal"]);
        specular.to_json(j["specular"]);
        ambient.to_json(j["ambient"]);
        shininess.to_json(j["shininess"]);
    }

    void CMaterial_Phong::from_json(const json& j)
    {
        CMaterial::from_json(j);
        diffuse.from_json(j["diffuse"]);
        normal.from_json(j["normal"]);
        specular.from_json(j["specular"]);
        ambient.from_json(j["ambient"]);
        shininess.from_json(j["shininess"]);
    }

    void CMaterial_Phong::set_shader_consts()
    {
        shader->use();
        shader->bind_ub("Camera", BindingPoint::CAMERA);
        shader->bind_ub("Lights", BindingPoint::LIGHT_SOURCES);
        shader->set("diffuse_tex.value", 0);
        shader->set("normal_tex.value", 1);
        shader->set("specular_tex.value", 2);
        shader->set("ambient_tex.value", 3);
        shader->set("shininess_tex.value", 4);
    }

    void CMaterial_Phong::draw(u32 lights_count)
    {
        Camera* main_camera = LINK_RENDERER->main_camera;

        shader->use();
        shader->set("model", transform ? transform->get_matrix() : glm::mat4(1));
        shader->set("lights_count", (int)lights_count);

        shader->set("viewPos", main_camera->position);

        diffuse.set_values(shader.get(), GL_TEXTURE0);
        normal.set_values(shader.get(), GL_TEXTURE1);
        specular.set_values(shader.get(), GL_TEXTURE2);
        ambient.set_values(shader.get(), GL_TEXTURE3);
        shininess.set_values(shader.get(), GL_TEXTURE4);
    }

    void CMaterial_Phong::set_texture(Texture2D* texture)
    {
        switch (texture->type)
        {
        case TextureType::DIFFUSE:
            diffuse.value = texture;
            diffuse.path = texture->file_path;
            break;
        case TextureType::NORMAL:
            normal.value = texture;
            normal.path = texture->file_path;
            break;
        case TextureType::SPECULAR:
            specular.value = texture;
            specular.path = texture->file_path;
            break;
            break;
        case TextureType::AMBIENT:
            ambient.value = texture;
            ambient.path = texture->file_path;
            break;
        case TextureType::SHININESS:
            shininess.value = texture;
            shininess.path = texture->file_path;
            break;
        }
    }


#ifdef LINK_EDITOR_ENABLED
    bool CMaterial_Phong::debug_draw()
    {
        if (!Component::debug_draw())
            return false;

        if (EUtils::Button("CMaterial_Phong", "Reload Shader"))
        {
            shader->reload();
            set_shader_consts();
        }

        diffuse.debug_draw();
        normal.debug_draw();
        specular.debug_draw();
        ambient.debug_draw();
        shininess.debug_draw();

        return true;
    }
#endif
}

