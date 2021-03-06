#include "c_light_source.hpp"

#include <imgui.h>
#include <fmt/ostream.h>
#include <assert.h>

#include "link/input.hpp"
#include "c_transform.hpp"
#include "c_material_phong.hpp"
#include "link/gfx/debug.hpp"
#include "link/gfx/renderer.hpp"
#include "link/gfx/camera.hpp"
#include "link/serialization.hpp"

namespace link
{
    LINK_IMPLEMENT_RTTI(link, CLightSource, Component);

    CLightSource::CLightSource(SceneObject* owner)
        : Component(owner, Type::LightSource)
        , transform(nullptr)
        , material(nullptr)
        , light{}
        , color{}
        , is_on(true)
        , cutoff(12.0f)
        , outer_cutoff(15.0f)
        , selected_attenuation(0)
    {
        LINK_RENDERER->subscribe(this);
    }

    CLightSource::~CLightSource()
    {
        LINK_RENDERER->unsubscribe(this);
    }

    void CLightSource::init()
    {
        refresh_dependances();
    }

    void CLightSource::refresh_dependances()
    {
        transform = get_component<CTransform>();
        material = get_component<CMaterial>();
        assert(transform && material);
    }

    void CLightSource::to_json(json& j)
    {
        Component::to_json(j);

        j["light"]["type"] = light.type;
        j["light"]["constant"] = light.constant;
        j["light"]["linear"] = light.linear;
        j["light"]["quadratic"] = light.quadratic;
        JsonUtils::to_json(j, color, "color");
        j["is_on"] = is_on;
        j["cutoff"] = cutoff;
        j["outer_cutoff"] = outer_cutoff;
        j["selected_attenuation"] = selected_attenuation;
    }

    void CLightSource::from_json(const json& j)
    {
        Component::from_json(j);

        light.type = j["light"]["type"];
        light.constant = j["light"]["constant"];
        light.linear = j["light"]["linear"];
        light.quadratic = j["light"]["quadratic"];
        JsonUtils::from_json(j, color, "color");
        is_on = j["is_on"];
        cutoff = j["cutoff"];
        outer_cutoff = j["outer_cutoff"];
        selected_attenuation = j["selected_attenuation"];
    }

    void CLightSource::update()
    {
        material->set_if_present("color", color);
    }

    void CLightSource::set_values(UBO& ubo, u32& offset, u32& count)
    {
        light.position = transform->position;
        light.direction = transform->forward();
        light.cutoff = glm::cos(glm::radians(cutoff));
        light.outer_cutoff = glm::cos(glm::radians(outer_cutoff));
        if (!is_on)
        {
            light.color.r = 0;
            light.color.g = 0;
            light.color.b = 0;
        }
        else
        {
            light.color = color;
        }
        ubo.set_data(offset, sizeof(light), &light);
        count++;
        offset += sizeof(light);
    }

#ifdef LINK_EDITOR_ENABLED
    void CLightSource::debug_update()
    {
        material->set_if_present("color", color);
    }

    bool CLightSource::debug_draw()
    {
        if (!Component::debug_draw())
            return false;

        bool changed = ImGui::Combo("Type", (i32*)&light.type, "Directional\0Point\0Spot\0\0");

        ImGui::Checkbox("Is on", &is_on);
        ImGui::ColorEdit3("Color", (float*)&color);

        if (ImGui::BeginCombo("Range", std::to_string(LightAttenuations[selected_attenuation].range).c_str(), 0))
        {
            for (int n = 0; n < IM_ARRAYSIZE(LightAttenuations); n++)
            {
                const bool is_selected = (selected_attenuation == n);
                if (ImGui::Selectable(std::to_string(LightAttenuations[n].range).c_str(), is_selected))
                {
                    selected_attenuation = n;
                }

                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        light.constant = LightAttenuations[selected_attenuation].constant;
        light.linear = LightAttenuations[selected_attenuation].linear;
        light.quadratic = LightAttenuations[selected_attenuation].quadratic;

        ImGui::DragFloat("Cutoff", &cutoff, 0.1f, 0.0f, 30.0f);
        ImGui::DragFloat("Outer Cutoff", &outer_cutoff, 0.1f, 0.0f, 30.0f);

        return true;
    }
#endif

}