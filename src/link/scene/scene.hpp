#pragma once


#include <vector>
#include <limits>
#include <string>
#include <memory>
#include <json.hpp>

#include "link/editor/editor.hpp"
#include "link/types.hpp"
#include "link/editor/e_string.hpp"


namespace link
{
    using nlohmann::json;

    struct Camera;
    struct SceneObject;
    struct Component;

    struct Scene
    {
        using SceneObjects = std::vector<std::unique_ptr<SceneObject>>;

        Scene(const std::string& name = "Default Scene Name");
        Scene(const std::string& path, bool from_path);

        ~Scene();

        void init();
        void update();
        void stop();
        void load();
        void save();

        SceneObject* create_object(const std::string& name = "New Object");
        void remove(SceneObject* obj);
        void remove_scene_object(u32 index);
        bool contains(SceneObject* obj);

#ifdef LINK_EDITOR_ENABLED
        u64 selected = U64_INVALID;
        void debug_update();
        void debug_draw();
#else
        void debug_update() {}
        inline void debug_draw() {}
#endif

        SceneObjects scene_objects;
        std::vector<SceneObject*> marked_for_remove;
        EString name;
        bool initialized;
        std::string path;

        void to_json(json& j);
        void from_json(const json& j);
    };


}