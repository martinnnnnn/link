#pragma once

#include <json.hpp>
#include <glm/glm.hpp>

namespace link
{
    namespace JsonUtils
    {
        using nlohmann::json;

        inline void to_json(json& j, const glm::vec3& vec3, const char* name)
        {
            j[name] = json::array();
            j[name][0] = vec3.x;
            j[name][1] = vec3.y;
            j[name][2] = vec3.z;
        }

        inline void from_json(const json& j, glm::vec3& vec3, const char* name)
        {
            vec3.x = j[name][0];
            vec3.y = j[name][1];
            vec3.z = j[name][2];
        }
    }
}