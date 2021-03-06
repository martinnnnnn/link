#include "renderer.hpp"


#include <imgui.h>

#include "camera.hpp"
#include "link/scene/components/c_model_static.hpp"
#include "link/scene/components/c_light_source.hpp"
#include "link/scene/components/c_transform.hpp"
#include "link/physics/collision.hpp"
#include "link/input.hpp"
#include "debug.hpp"

namespace link
{
    Renderer::Renderer()
        : main_camera(nullptr)
        , editor_position {}
        , viewport_size {}
    {
        main_camera = new Camera();
        main_camera->update_view();

        light_sources_ubo.create(MAX_LIGHTS_SOURCES * sizeof(LightSource), BindingPoint::LIGHT_SOURCES);
        light_sources_ubo.bind_range(BindingPoint::LIGHT_SOURCES, 0, MAX_LIGHTS_SOURCES * sizeof(LightSource));

        //pbr_lights_ubo.create(MAX_PBR_LIGHTS * sizeof(LightSource_PBR), BindingPoint::PBR_LIGHTS);
        //pbr_lights_ubo.bind_range(BindingPoint::PBR_LIGHTS, 0, MAX_PBR_LIGHTS * sizeof(LightSource_PBR));
    }

    void Renderer::subscribe(CModel_Static* model)
    {
        models_static.insert(model);
    }

    void Renderer::unsubscribe(CModel_Static* model)
    {
        models_static.erase(models_static.find(model));
    }

    void Renderer::subscribe(CLightSource* light_source)
    {
        light_sources.insert(light_source);
    }

    void Renderer::unsubscribe(CLightSource* light_source)
    {
        light_sources.erase(light_sources.find(light_source));
    }

    void Renderer::bind()
    {
        if (framebuffer.fbo)
        {
            framebuffer.bind();
        }
    }

    void Renderer::unbind()
    {
        if (framebuffer.fbo)
        {
            framebuffer.unbind();
            imgui_swap();
        }
    }

    void Renderer::draw()
    {
        u32 offset = 0;
        u32 count = 0;
        for (CLightSource* light : light_sources)
        {
            light->set_values(light_sources_ubo, offset, count);
        }

        for (CModel_Static* model : models_static)
        {
            if (LINK_INPUT->mouse.down(MouseButton::LEFT))
            {
                if (Physics::check_collision(main_camera->screen_to_world(), { model->get_component<CTransform>()->position, 1.0f }).x != -1.0f)
                {
                    static int times = 0;
                    fmt::print("intersection !!! {}\n", times++);
                }
            }

            model->draw(light_sources, count);
        }
    }

    void Renderer::imgui_swap()
    {
        ImGui::Begin("Scene");

        ImVec2 position = ImGui::GetWindowContentRegionMin();
        position.x += ImGui::GetWindowPos().x;
        position.y += ImGui::GetWindowPos().y;
        editor_position = { position.x, position.y };

        const ImVec2 imgui_panel_size = ImGui::GetContentRegionAvail();
        const glm::vec2 panel_size{ imgui_panel_size.x, imgui_panel_size.y };

        update_framebuffer_size(panel_size);

        u64 texture_id = framebuffer.texture;

        ImGui::Image(reinterpret_cast<void*>(texture_id), imgui_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();
    }

    void Renderer::update_framebuffer_size(const glm::vec2& size)
    {
        if (size != framebuffer.size)
        {
            to_framebuffer(size);
            main_camera->update_projection(size);
        }
    }

    void Renderer::to_framebuffer(const glm::vec2& size)
    {
        viewport_size = size;
        framebuffer.init(viewport_size);
    }

    void Renderer::to_viewport(const glm::vec2& size)
    {
        framebuffer.clear();
        viewport_size = size;
        main_camera->update_projection(viewport_size);
    }
}