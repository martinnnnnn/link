#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/ostream.h>

#include "link/input.hpp"
#include "link/timer.hpp"
#include "renderer.hpp"
#include "debug.hpp"

namespace link
{
    Camera::Camera()
        : position(0.0f, 0.0f, 3.0f)
        , front(0.0f, 0.0f, -1.0f)
        , up(0.0f, 1.0f, 0.0f)
        , yaw(-90.0f)
        , pitch(0.0f)
        , speed(50.0f)
        , mouse_sensitivity(10.0f)
        , view(glm::mat4(1))
        , projection(glm::mat4(1))
    {
        ubo.create(2 * sizeof(glm::mat4), BindingPoint::CAMERA);
        ubo.bind_range(BindingPoint::CAMERA, 0, 2 * sizeof(glm::mat4));
    }

    void Camera::update()
    {
        const float delta_time = link::GlobalTimer::get()->timer.dt_s;

        // updating camera mouvement
        speed += (i32)Input::get()->mouse.wheel * 5;
        speed = speed < 5 ? speed = 5 : speed;

        const f32 scaled_speed = speed * delta_time;
        if (Input::get()->is_down(SDL_SCANCODE_W))
        {
            position += scaled_speed * front;
        }
        if (Input::get()->is_down(SDL_SCANCODE_S))
        {
            position -= scaled_speed * front;
        }
        if (Input::get()->is_down(SDL_SCANCODE_A))
        {
            position -= glm::normalize(glm::cross(front, up)) * scaled_speed;
        }
        if (Input::get()->is_down(SDL_SCANCODE_D))
        {
            position += glm::normalize(glm::cross(front, up)) * scaled_speed;
        }

        // updating camera direction
        f32 xoffset = Input::get()->mouse.offset.x * mouse_sensitivity * delta_time;
        f32 yoffset = -Input::get()->mouse.offset.y * mouse_sensitivity * delta_time;

        yaw += xoffset;
        pitch += yoffset;

        pitch = pitch > 89.0f ? pitch = 89.0f : pitch;
        pitch = pitch < -89.0f ? pitch = -89.0f : pitch;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);

        update_view();
    }

    void Camera::update_view()
    {
        view = glm::lookAt(position, position + front, up);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        ubo.set_data(0, sizeof(glm::mat4), glm::value_ptr(view));
    }

    void Camera::update_projection(const glm::vec2& scene_render_size)
    {
        projection = glm::perspective(glm::radians(45.0f), scene_render_size.x / scene_render_size.y, 0.1f, 10000.0f);
        ubo.set_data(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    }

    Ray Camera::screen_to_world()
    {
        // normalized device coordinates ( [-1:1, -1:1, -1:1] )
        glm::vec4 ray_start_ndc(
            (2.0f * f32(LINK_INPUT->mouse.position.x)) / f32(LINK_RENDERER->viewport_size.x) - 1.0f,
            1.0f - (2.0f * f32(LINK_INPUT->mouse.position.y)) / f32(LINK_RENDERER->viewport_size.y),
            -1.0,
            1.0f );

        glm::vec4 ray_end_ndc(ray_start_ndc.x, ray_start_ndc.y, 0.0, 1.0f);

        glm::mat4 inv = glm::inverse(projection * view);

        glm::vec4 ray_start_world = inv * ray_start_ndc;
        ray_start_world /= ray_start_world.w;

        glm::vec4 ray_end_world = inv * ray_end_ndc;
        ray_end_world /= ray_end_world.w;

        return Ray{ glm::vec3(ray_start_world), glm::normalize(ray_end_world - ray_start_world) };
    }
}