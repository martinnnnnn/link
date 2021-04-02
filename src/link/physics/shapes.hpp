#pragma once

#include <glm/glm.hpp>

#include "link/types.hpp"

namespace link
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    struct Sphere
    {
        glm::vec3 center;
        f32 radius;
    };
}