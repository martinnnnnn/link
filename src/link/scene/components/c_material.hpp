#pragma once

#include <memory>
#include <json.hpp>
#include <glm/glm.hpp>

#include "link/types.hpp"
#include "link/scene/component.hpp"
#include "link/editor/editor.hpp"
#include "link/editor/e_uniform.hpp"

namespace link
{
    struct Shader;
    struct CTransform;
    struct Texture2D;

    struct CMaterial : public Component
    {
        CMaterial(SceneObject* owner, Type type = Type::Material);

        void init() override;
        void refresh_dependances() override;

        void load(const std::string& vertex_path, const std::string& fragment_path);

        void set_if_present(const std::string& name, const glm::vec3& value);
        void set_if_present(const std::string& name, const f32 value);

        void to_json(json& j) override;
        void from_json(const json& j) override;

        virtual void set_shader_consts();
        virtual void draw(u32 lights_count);
        virtual void set_texture(Texture2D* texture) {}

        std::unique_ptr<Shader> shader;
        CTransform* transform;
        std::vector<EUniform> euniforms;

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw() override;
#else
        inline bool debug_draw() { return false; }
#endif

        //NLOHMANN_DEFINE_TYPE_INTRUSIVE(CMaterial, path_vertex, path_fragment, path_texture_diffuse, path_texture_specular)
        LINK_DECLARE_RTTI
    };
}
