#pragma once


#include <glm/glm.hpp>
#include <SDL_video.h>

#include "link/singleton.hpp"
struct SDL_Window;

namespace link
{

    struct Window : Singleton<Window>
    {
        glm::ivec2 position;
        glm::ivec2 size;
        SDL_Window* window;
        SDL_GLContext gl_context;

        void init(const glm::ivec2& window_size);
        void shutdown();
        void on_resize(const glm::ivec2& window_size);
        void on_move(const glm::ivec2& new_position);
    };
}

#define LINK_WINDOW link::Window::get()

