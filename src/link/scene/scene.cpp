#include "scene.hpp"

#include <fmt/ostream.h>

#ifdef LINK_EDITOR_ENABLED
#include <imgui.h>
#endif LINK_EDITOR_ENABLED

#include "scene_object.hpp"
#include "component.hpp"
#include "link/gfx/camera.hpp"
#include "link/gfx/renderer.hpp"
#include "game.hpp"

namespace link
{
    Scene::~Scene() = default;

    Scene::Scene(const std::string& n)
        : name("Scene name", n)
        , initialized(false)
    {
        path = fmt::format("{}{}{}.link", LINK_DATA_ROOT, "scenes/", name.value);
        if (link::File::exists(path))
        {
            load();
        }
        else
        {
            save();
        }
    }

    Scene::Scene(const std::string& path, bool from_path)
        : name("Scene name", "")
        , initialized(false)
        , path(path)
    {
        if (link::File::exists(path))
        {
            load();
        }
    }


    void Scene::init()
    {
        for (auto& obj : scene_objects)
        {
            obj->init();
        }
        initialized = true;
    }

    void Scene::update()
    {
        // starting base in case we add a selected flag in scene object and dont need index during removal
        //for (auto it = scene_objects.begin(); it != scene_objects.end();)
        //{
        //    bool deleted = false;
        //    for (SceneObject* obj : marked_for_remove)
        //    {
        //        if (it->get() == obj)
        //        {
        //            it = scene_objects.erase(it);
        //            deleted = true;
        //            break;
        //        }
        //        //else
        //        //{
        //        //    ++it;
        //        //}
        //    }
        //    if (!deleted)
        //    {

        //    }
        //}

        for (u32 i = 0; i < scene_objects.size();)
        {
            bool deleted = false;
            for (SceneObject* obj : marked_for_remove)
            {
                if (scene_objects[i].get() == obj)
                {
                    deleted = true;
                    remove_scene_object(i);
#ifdef LINK_EDITOR_ENABLED
                    if (selected > i)
                    {
                        selected--;
                    }
                    else if (selected == i)
                    {
                        selected = U64_INVALID;
                    }
#endif
                    break;
                }
            }
            if (!deleted) ++i;
        }

        marked_for_remove.clear();

        for (auto& obj : scene_objects)
        {
            obj->update();
        }
    }

    void Scene::stop() 
    {
        initialized = false;
        for (auto& obj : scene_objects)
        {
            obj->stop();
        }
    }

    void Scene::load()
    {
        using nlohmann::json;

        scene_objects.clear();

        std::string content;
        //link::File::read(path, content);
        File2::read(path, content);

        json json_scene = json::parse(content);
        from_json(json_scene);
    }

    void Scene::save()
    {
        using nlohmann::json;

        json json_scene;
        to_json(json_scene);

        //std::string dump = json_scene.dump(4);
        //link::File::write(path, (u8*)dump.c_str(), dump.length());
        File2::write(path, json_scene.dump(4));
    }

    SceneObject* Scene::create_object(const std::string& name)
    {
        SceneObject* new_obj = scene_objects.emplace_back(std::make_unique<SceneObject>(this, name)).get();
        new_obj->refresh_dependencies();
        if (initialized)
        {
            new_obj->init();
        }
        return new_obj;
    }

    void Scene::remove(SceneObject* obj)
    {
        marked_for_remove.push_back(obj);
    }

    void Scene::remove_scene_object(u32 index)
    {
        scene_objects.erase(scene_objects.begin() + index);
    }

    bool Scene::contains(SceneObject* obj1)
    {
        for (auto& obj2 : scene_objects)
        {
            if (obj1 == obj2.get())
            {
                return true;
            }
        }
        return false;
    }

    void Scene::to_json(json& j)
    {
        name.to_json(j["name"], false);

        j["scene_objects"] = json::array();
        for (u32 i = 0; i < scene_objects.size(); ++i)
        {
            scene_objects[i]->to_json(j["scene_objects"][i]);
        }
    }

    void Scene::from_json(const json& j)
    {
        name.from_json(j.at("name"), false);

        for (u32 i = 0; i < j["scene_objects"].size(); ++i)
        {
            SceneObject* obj = create_object();
            obj->from_json(j["scene_objects"][i]);
        }
    }

#ifdef LINK_EDITOR_ENABLED

    void Scene::debug_update()
    {
        for (i32 i = 0; i < scene_objects.size(); ++i)
        {
            scene_objects[i]->debug_update();
        }
    }

    void Scene::debug_draw()
    {
        const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (ImGui::Begin("Scene Editor"))
        {
            if (ImGui::TreeNode(name.value.c_str()))
            {
                for (i32 i = 0; i < scene_objects.size(); ++i)
                {
                    ImGuiTreeNodeFlags node_flags = base_flags;

                    if (i == selected)
                    {
                        node_flags |= ImGuiTreeNodeFlags_Selected;
                    }

                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", scene_objects[i]->name.value.c_str());
                    if (ImGui::IsItemClicked())
                    {
                        selected = i;
                    }
                }
                ImGui::TreePop();
            }

            if (LINK_GAME->state == Game::State::Stoped)
            {
                //if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered())
                if (ImGui::BeginPopupContextItem(name.value.c_str()))
                {
                    if (EUtils::Button("Scene", "New Object", { -1, -1, -1 }, { 120, 0 }))
                    {
                        create_object();
                    }

                    if (EUtils::Button("Scene", "Save", { -1, -1, -1 }, { 120, 0 }))
                    {
                        save();
                    }

                    if (EUtils::Button("Scene", "Load", { -1, -1, -1 }, { 120, 0 }))
                    {
                        load();
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::End();
        }

        if (selected != U64_INVALID)
        {
            scene_objects[selected]->debug_draw();
        }
        else
        {
            ImGui::Begin("Properties");
            ImGui::End();
        }
    }
#endif
}