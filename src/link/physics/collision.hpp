#pragma once

#include <glm/glm.hpp>

#include "shapes.hpp"

namespace link
{
    namespace Physics
    {
        glm::vec2 check_collision(const Ray& ray, const Sphere& sphere)
        {
            glm::vec3 oc = ray.origin - sphere.center;
            float b = dot(oc, ray.direction);
            float c = dot(oc, oc) - sphere.radius * sphere.radius;
            float h = b * b - c;
            if (h < 0.0) return glm::vec2{ -1.0f, -1.0f }; // no intersection
            h = sqrt(h);
            return glm::vec2{ -b - h, -b + h };
        }
    }
}