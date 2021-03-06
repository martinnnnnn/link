#include "component.hpp"

#include <imgui.h>

#include "scene_object.hpp"

namespace link
{
    LINK_IMPLEMENT_RTTI(link, Component, RttiObject);


    void Component::to_json(json& j)
    {
        j["type"] = type;
    }

    void Component::from_json(const json& j)
    {
        type = j["type"];
    }

#ifdef LINK_EDITOR_ENABLED
    bool Component::debug_draw()
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(TypeColors[(u32)type].r, TypeColors[(u32)type].g, TypeColors[(u32)type].b, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(TypeColors[(u32)type].r, TypeColors[(u32)type].g, TypeColors[(u32)type].b, 0.7f));
        bool is_open = ImGui::CollapsingHeader(TypeNames[(u32)type]);
        ImGui::PopStyleColor(2);

        if (is_open)
        {
            ImGui::PushID("ComponentScope");
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.0f, 0.0f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
            if (ImGui::Button("Delete", ImVec2(120, 0)))
            {
                owner->remove(this);
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        return is_open;
    }
#endif
}

