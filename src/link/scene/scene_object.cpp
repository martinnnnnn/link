#include "scene_object.hpp"

#include <fmt/ostream.h>
#include <glm/glm.hpp>
#include <algorithm>

#include "component.hpp"
#include "scene.hpp"
#include "components/c_transform.hpp"
#include "components/c_material.hpp"
#include "components/c_material_phong.hpp"
#include "components/c_material_pbr.hpp"
#include "components/c_model_static.hpp"
#include "components/c_light_source.hpp"

#ifdef LINK_EDITOR_ENABLED
#include <imgui.h>
#endif LINK_EDITOR_ENABLED

namespace link
{
    void SceneObject::init()
    {
        for (auto& component : components)
        {
            component.second->init();
        }
        initialized = true;
    }

    void SceneObject::update()
    {
        real_remove();

        for (auto& component : components)
        {
            component.second->update();
        }
    }

    void SceneObject::stop()
    {
        initialized = false;
    }

    void SceneObject::refresh_dependances()
    {
        for (auto& component : components)
        {
            component.second->refresh_dependances();
        }
    }

    void SceneObject::remove(Component* component)
    {
        marked_for_remove.emplace_back(component);
    }

    void SceneObject::real_remove()
    {
        for (u32 i = 0; i < marked_for_remove.size(); ++i)
        {
            for (auto it = components.begin(); it != components.end();)
            {
                if (it->second.get() == marked_for_remove[i])
                {
                    it = components.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        marked_for_remove.clear();
    }

    bool SceneObject::contains(Component* comp)
    {
        for (auto& component : components)
        {
            if (component.second.get() == comp)
            {
                return true;
            }
        }
        return false;
    }

    void SceneObject::to_json(json& j)
    {
        name.to_json(j["name"], false);
        j["components"] = json::array();

        u32 index = 0;
        for (auto& kv : components)
        {
            kv.second->to_json(j["components"][index++]);
        }
    }

    void SceneObject::from_json(const json& j)
    {
        name.from_json(j.at("name"), false);

        for (u32 i = 0; i < j["components"].size(); ++i)
        {
            Component::Type type = j["components"][i]["type"];
            switch (type)
            {
            case Component::Type::Transform:
            {
                CTransform* component = add_component<CTransform>();
                break;
            }
            case Component::Type::Material:
            {
                CMaterial* component = add_component<CMaterial>();
                break;
            }
            case Component::Type::Material_PBR:
            {
                CMaterial_PBR* component = add_component<CMaterial_PBR>();
                break;
            }
            case Component::Type::Material_Phong:
            {
                CMaterial_Phong* component = add_component<CMaterial_Phong>();
                break;
            }
            case Component::Type::Model_Static:
            {
                CModel_Static* component = add_component<CModel_Static>();
                break;
            }
            case Component::Type::LightSource:
            {
                CLightSource* component = add_component<CLightSource>();
                break;
            }
            }
        }

        for (u32 i = 0; i < j["components"].size(); ++i)
        {
            Component::Type type = j["components"][i]["type"];
            switch (type)
            {
            case Component::Type::Transform:
            {
                CTransform* component = get_component<CTransform>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            case Component::Type::Material:
            {
                CMaterial* component = get_component<CMaterial>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            case Component::Type::Material_PBR:
            {
                CMaterial_PBR* component = get_component<CMaterial_PBR>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            case Component::Type::Material_Phong:
            {
                CMaterial_Phong* component = get_component<CMaterial_Phong>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            case Component::Type::Model_Static:
            {
                CModel_Static* component = get_component<CModel_Static>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            case Component::Type::LightSource:
            {
                CLightSource* component = get_component<CLightSource>();
                component->refresh_dependances();
                component->from_json(j["components"][i]);
                break;
            }
            }
        }
    }

#ifdef LINK_EDITOR_ENABLED

    void SceneObject::debug_update()
    {
        for (auto& component : components)
        {
            component.second->debug_update();
        }
    }

    void SceneObject::debug_draw()
    {
        ImGui::Begin("Properties");

        name.debug_draw();

        if (EUtils::Button("SceneObjectScope", "Delete", { 0.5, 0, 0 }, { 120, 0 }))
        {
            owner->remove(this);
        }

        ImGui::SameLine();
        if (ImGui::Button("Add Component", ImVec2(120, 0)))
        {
            ImGui::OpenPopup(link::Editor::COMPONENT_CREATION_WINDOW);
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(link::Editor::COMPONENT_CREATION_WINDOW, NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const char* items[] = { "Transform", "Material", "Phong Material", "PBR Material", "Static Model", "Light Source" };
            static int item_current = 0;
            ImGui::ListBox("Component Type", &item_current, items, IM_ARRAYSIZE(items), 10);

            if (EUtils::Button("SceneObjectScope", "Create", { 0, 0.5, 0 }, { 120, 0 }))
            {
                switch (item_current)
                {
                case 0:
                    add_component<CTransform>();
                    break;
                case 1:
                {
                    CMaterial* new_material = add_component<CMaterial>();
                    new_material->load(std::string(LINK_DATA_ROOT) + "glsl/static.vs", std::string(LINK_DATA_ROOT) + "glsl/solid_color.fs");
                    break;
                }
                case 2:
                {
                    CMaterial_Phong * new_material = add_component<CMaterial_Phong>();
                    new_material->load(std::string(LINK_DATA_ROOT) + "glsl/static.vs", std::string(LINK_DATA_ROOT) + "glsl/phong.fs");
                    break;
                }
                case 3:
                {
                    CMaterial_PBR * new_material = add_component<CMaterial_PBR>();
                    new_material->load(std::string(LINK_DATA_ROOT) + "glsl/static.vs", std::string(LINK_DATA_ROOT) + "glsl/pbr.fs");
                    break;
                }
                case 4:
                    add_component<CModel_Static>();
                    break;
                case 5:
                    add_component<CLightSource>();
                    break;
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (EUtils::Button("SceneObjectScope", "Cancel", { 0.5, 0, 0 }, { 120, 0 }))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        u32 i = 0;
        for (auto& component : components)
        {
            ImGuiID child_id = ImGui::GetID((void*)(intptr_t)i++);
            component.second->debug_draw();
        }
        ImGui::End();
    }
#endif
}
