#include "e_texture_2d.hpp"

#include <fmt/ostream.h>

#include "link/data_system.hpp"
#include "link/gfx/shader.hpp"

#ifdef LINK_EDITOR_ENABLED
#include <imgui.h>
#endif LINK_EDITOR_ENABLED

namespace link
{
    ETexture2D::ETexture2D(const std::string& name, const std::string& shader_name, TextureType type, bool single_value_default, const glm::vec3& default_value)
        : shader_name(shader_name)
        , name(name)
        , value(nullptr)
        , default_value(default_value)
        , type(type)
        , single_value_default(single_value_default)
    {}

    void ETexture2D::set_values(Shader* shader, i32 texture_unit)
    {
        int set = (value != nullptr && value->id != 0) ? 1 : 0;
        shader->set((shader_name + ".set"), set);
        if (set)
        {
            value->bind(texture_unit);
        }
        else
        {
            if (single_value_default)
            {
                shader->set((shader_name + ".default_value"), default_value.r);
            }
            else
            {
                shader->set((shader_name + ".default_value"), default_value);
            }
        }
    }

    void ETexture2D::to_json(json& j)
    {
        j["path"] = Path::get_path_relative(LINK_DATA_ROOT, path);
        JsonUtils::to_json(j, default_value, "default_value");
        j["single_value_default"] = single_value_default;
    }

    void ETexture2D::from_json(const json& j)
    {
        path = std::string(LINK_DATA_ROOT).append(j["path"]);
        JsonUtils::from_json(j, default_value, "default_value");
        single_value_default = j["single_value_default"];

        if (!path.empty())
        {
            value = LINK_DATA_SYSTEM->load_texture_2d(path, type);
        }
    }

#ifdef LINK_EDITOR_ENABLED
    bool ETexture2D::debug_draw()
    {
        ImGui::Text(name.c_str());
        u64 tex_id = (value && value->id) ? u64(value->id) : u64(LINK_DATA_SYSTEM->get_default_texture()->id);
        ImVec2 size = ImVec2(80, 80);
        ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
        ImGui::Image((void*)tex_id, size, uv_min, uv_max, tint_col, border_col);

        static std::string previous_path = path;
        previous_path = path;

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND::c_str(DND::Target::FilePath)))
            {
                uptr* ptr = (uptr*)(payload->Data);
                FileSystem::Node*  fs_node = (FileSystem::Node*)(*ptr);
                path = fs_node->get_absolute();
            }
            else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DND::c_str(DND::Target::String)))
            {
                path = std::string((char*)payload->Data, payload->DataSize);
            }
            ImGui::EndDragDropTarget();
        }

        if (path.compare(previous_path) != 0)
        {
            value = LINK_DATA_SYSTEM->load_texture_2d(path, type);
        }

        //bool has_changed = false;
        //has_changed = path.debug_draw();
        //if (has_changed)
        //{
        //    value = LINK_DATA_SYSTEM->load_texture_2d(path.value, type);
        //}
        if (!value)
        {
            if (single_value_default)
            {
                ImGui::DragFloat("", (float*)&default_value, 0.01f, 0.0f, 1.0f);
            }
            else
            {
                ImGui::SameLine();
                ImGui::ColorEdit3("", (float*)&default_value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            }
        }

        return false;
    }
#endif
}
