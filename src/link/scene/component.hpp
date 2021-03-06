#pragma once


#include <json.hpp>

#include "link/type_id.hpp"
#include "link/rtti.hpp"
#include "link/rtti_object.hpp"
#include "scene_object.hpp"
#include "link/editor/editor.hpp"

using ComponentId = u32;

namespace link
{
    struct Component : public RttiObject
    {
        using ComponentTypeId = type_id<struct COMPONENT_TYPE_ID>;

        enum class Type
        {
            Transform,
            Material,
            Material_PBR,
            Material_Phong,
            Model_Static,
            LightSource,
            Count
        };

        static constexpr char* TypeNames[]
        {
            "Transform",
            "Material",
            "PBR Material",
            "Phong Material",
            "Static Model",
            "Light Source",
        };

        static constexpr glm::vec3 TypeColors[]
        {
            {119.0f / 255.f, 175.0f / 255.f, 182.0f / 255.f},
            {211.0f / 255.f, 243.0f / 255.f, 238.0f / 255.f},
            {218.0f / 255.f, 204.0f / 255.f, 62.0f / 255.f},
            {188.0f / 255.f, 44.0f / 255.f, 26.0f / 255.f},
            {125.0f / 255.f, 21.0f / 255.f, 56.0f / 255.f},
            {187.0f / 255.f, 133.0f / 255.f, 136.0f / 255.f},
        };

        Component(SceneObject* owner, Type type) : owner(owner), type(type) {}
        virtual ~Component() {}

        virtual void            init() {};
        virtual void            update() {};
        virtual void            refresh_dependances() {};

        virtual void to_json(json& j);
        virtual void from_json(const json& j);

        template<typename T>
        T* get_component();

        template<typename T>
        T* add_component();

        template<typename T>
        static ComponentId type_id();

        SceneObject* owner;
        Type type;

#ifdef LINK_EDITOR_ENABLED
        virtual void debug_update() {}
        virtual bool debug_draw();
#else
        inline void debug_update() {}
        inline bool debug_draw() { return false; }
#endif

        LINK_DECLARE_RTTI
    };

    template<typename T>
    T* Component::get_component()
    {
        return owner->get_component<T>();
    }

    template<typename T>
    T* Component::add_component()
    {
        return owner->add_component<T>();
    }

    template<typename T>
    ComponentId Component::type_id()
    {
        static_assert(std::is_base_of<Component, T>::value, "This is to get type IDs of components");
        return ComponentTypeId::get<T>();
    }
}
