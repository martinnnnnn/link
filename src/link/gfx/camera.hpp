#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "link/types.hpp"
#include "ubo.hpp"
#include "link/physics/shapes.hpp"

namespace link
{
    struct Camera
    {
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        f32	yaw;
        f32	pitch;
        f32	speed;
        f32	mouse_sensitivity;

        glm::mat4 view;
        glm::mat4 projection;

        UBO ubo;

        Camera();
        void update();
        void update_projection(const glm::vec2& scene_render_size);
        void update_view();


        Ray screen_to_world();
    };
}