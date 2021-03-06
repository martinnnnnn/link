#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <json.hpp>

#include "link/types.hpp"
#include "link/rtti_object.hpp"
#include "link/editor/editor.hpp"
#include "link/editor/e_string.hpp"

namespace link
{
    using ComponentId = u32;

    struct Component;
    struct Scene;

    struct SceneObject
    {
        using Components = std::unordered_map<ComponentId, std::unique_ptr<Component>>;

        SceneObject(Scene* scene, const std::string& name)
            : owner(scene)
            , name("", name)
            , initialized(false)
        {
        }

        void init();
        void update();
        void stop();
        void refresh_dependances();

        template<typename T>
        T* add_component();

        template<typename T>
        T* get_component();

        void remove(Component* component);
        bool contains(Component* component);

        Components components;
        Scene* owner;
        EString name;
        bool initialized;

    private:
        std::vector<Component*> marked_for_remove;
        void real_remove();
    public:

        void to_json(json& j);
        void from_json(const json& j);

#ifdef LINK_EDITOR_ENABLED
        void debug_update();
        void debug_draw();
#else
        void debug_update() {}
        inline void debug_draw() {}
#endif
    };

    template<typename T>
    T* SceneObject::get_component()
    {
        static_assert(std::is_base_of<Component, T>::value, "You can only acquire objects from Component derived classes");

        ComponentId id = Component::type_id<T>();

        if (components.find(id) != components.end())
        {
            return static_cast<T*>(components[id].get());
        }

        for (auto& component : components)
        {
            //if (T::TYPE.is_derived(component.second->get_type()))
            if (component.second->is_derived_of(T::TYPE))
            {
                return static_cast<T*>(component.second.get());
            }
        }

        return nullptr;
    }

    template<typename T>
    T* SceneObject::add_component()
    {
        static_assert(std::is_base_of<Component, T>::value, "You can only acquire objects from Component derived classes");
        assert(get_component<T>() == nullptr && "A component of this type already exists in the object");

        ComponentId id = Component::type_id<T>();
        auto result = components.insert(std::make_pair(id, std::make_unique<T>(this)));

        assert(result.first != components.end() && result.second);

        T* new_component = static_cast<T*>(result.first->second.get());
        if (initialized)
        {
            new_component->init();
        }

        return new_component;
    }
}

// 09 77 41 03 81