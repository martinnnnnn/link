#include "c_material.hpp"

#include <fmt/ostream.h>
#include <imgui.h>

#include "link/gfx/shader.hpp"
#include "link/gfx/ubo.hpp"
#include "link/gfx/renderer.hpp"
#include "link/gfx/camera.hpp"
#include "link/timer.hpp"
#include "c_transform.hpp"

namespace link
{
    LINK_IMPLEMENT_RTTI(link, CMaterial, Component);

    CMaterial::CMaterial(SceneObject* owner, Type type)
        : Component(owner, type),
        transform(nullptr)
    {
        shader = std::make_unique<Shader>();
    }

    void CMaterial::init()
    {
        refresh_dependances();
    }

    void CMaterial::refresh_dependances()
    {
        transform = get_component<CTransform>();
        assert(transform);
    }

    void CMaterial::load(const std::string& vertex_path, const std::string& fragment_path)
    {
        shader->load(vertex_path, fragment_path);
        set_shader_consts();
    }

    void CMaterial::set_if_present(const std::string& name, const glm::vec3& value)
    {
        for (EUniform& euniform : euniforms)
        {
            if (euniform.uniform.name.compare(name) == 0 && euniform.uniform.type == GL_FLOAT_VEC3)
            {
                euniform.vec3 = value;
                break;
            }
        }
    }

    void CMaterial::set_if_present(const std::string& name, const f32 value)
    {
        for (EUniform& euniform : euniforms)
        {
            if (euniform.uniform.name.compare(name) == 0 && euniform.uniform.type == GL_FLOAT_VEC3)
            {
                euniform.float32 = value;
                break;
            }
        }
    }


    void CMaterial::to_json(json& j)
    {
        Component::to_json(j);

        j["vertex"] = Path::get_path_relative(LINK_DATA_ROOT, shader->vertex_path);
        j["fragment"] = Path::get_path_relative(LINK_DATA_ROOT, shader->fragment_path);

        for (EUniform euniform : euniforms)
        {
            euniform.to_json(j[euniform.uniform.name]);
        }
    }

    void CMaterial::from_json(const json& j)
    {
        Component::from_json(j);

        load(std::string(LINK_DATA_ROOT).append(j["vertex"]), std::string(LINK_DATA_ROOT).append(j["fragment"]));

        for (EUniform euniform : euniforms)
        {
            euniform.from_json(j[euniform.uniform.name]);
        }
    }

    void CMaterial::set_shader_consts()
    {
        shader->use();
        shader->bind_ub("Camera", BindingPoint::CAMERA);

        std::vector<Uniform> uniforms = shader->get_uniforms();
        for (Uniform& uniform : uniforms)
        {
            euniforms.emplace_back(uniform);
        }
    }

    void CMaterial::draw(u32 lights_count)
    {
        shader->use();
        shader->set("model", transform->get_matrix());

        for (EUniform& euniform : euniforms)
        {
            switch (euniform.uniform.type)
            {
            case GL_FLOAT:
                {
                    shader->set(euniform.uniform.id, euniform.float32);
                    break;
                }
            case GL_FLOAT_VEC3:
                {
                    shader->set(euniform.uniform.id, euniform.vec3);
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
        }
    }

#ifdef LINK_EDITOR_ENABLED
    bool CMaterial::debug_draw()
    {
        if (!Component::debug_draw())
            return false;

        if (ImGui::Button("Reload Shader"))
        {
            shader->reload();
            set_shader_consts();
        }

        for (EUniform& euniform : euniforms)
        {
            euniform.debug_draw();
        }
        return true;
    }
#endif
}
