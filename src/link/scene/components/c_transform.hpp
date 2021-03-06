#pragma once

#include <glm/glm.hpp>

#include "link/scene/component.hpp"
#include "link/editor/editor.hpp"

namespace link
{
    struct CTransform : Component
    {
        CTransform(SceneObject* owner) : Component(owner, Type::Transform), position(), rotation(), scale(1, 1, 1) {}
        //void init() override;
        //void shutdown() override;
        //void update() override;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 get_matrix();
        glm::vec3 CTransform::forward();

        void to_json(json& j) override;
        void from_json(const json& j) override;

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw() override;
#else
        inline bool debug_draw() { return false; }
#endif

        LINK_DECLARE_RTTI
    };
}
