#include "window.hpp"

#include <GL/glew.h>
#include <gl/GL.h>
// SDL
#include <SDL.h>
#include <SDL_syswm.h>

#include <fmt/ostream.h>

namespace link
{
    void Window::init(const glm::ivec2& window_size)
    {
        SDL_SetMainReady();
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
        {
            fmt::print(stderr, "SDL Init error: {}\n", SDL_GetError());
            return;
        }

        const char* glsl_version = "#version 420";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        size = window_size;

        window = SDL_CreateWindow("Link", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, window_flags);

        SDL_GetWindowPosition(window, &position.x, &position.y);

        gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            fmt::print(stderr, "Error: {}\n", glewGetErrorString(err));
        }

        glViewport(0, 0, window_size.x, window_size.y);

        glEnable(GL_DEPTH_TEST);
    }


    void Window::on_resize(const glm::ivec2& window_size)
    {
        size = window_size;
        glViewport(0, 0, size.x, size.y);
    }

    void Window::on_move(const glm::ivec2& new_position)
    {
        position = new_position;
    }

    void Window::shutdown()
    {
        SDL_GL_DeleteContext(LINK_WINDOW->gl_context);
        SDL_DestroyWindow(LINK_WINDOW->window);
        SDL_Quit();
    }

}
