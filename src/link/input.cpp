#include "input.hpp"

#include <SDL.h>
#include <fmt/ostream.h>

#include "window.hpp"
#include "link/editor/editor.hpp"
#include "gfx/renderer.hpp"
#include "gfx/camera.hpp"

namespace link
{
    Mouse::Mouse()
        : position{}
        , offset{}
        , buttons_mask(0)
        , wheel(MouseWheel::NONE)
    {}

    void Mouse::update()
    {
        //buttons_mask = SDL_GetRelativeMouseState(&offset.x, &offset.y);
        static glm::ivec2 viewport_position;
        static glm::ivec2 last_position = viewport_position;
        last_position = viewport_position;

        buttons_mask = SDL_GetMouseState(&viewport_position.x, &viewport_position.y);
        offset = viewport_position - last_position;

#ifdef LINK_EDITOR_ENABLED
        position = viewport_position - (LINK_RENDERER->editor_position - LINK_WINDOW->position);
#else
        position = viewport_position;
#endif
    }

    bool Mouse::down(MouseButton button)
    {
        switch (button)
        {
        case MouseButton::LEFT:
            return buttons_mask & SDL_BUTTON(SDL_BUTTON_LEFT);
        case MouseButton::MIDDLE:
            return buttons_mask & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        case MouseButton::RIGHT:
            return buttons_mask & SDL_BUTTON(SDL_BUTTON_RIGHT);
        case MouseButton::X1:
            return buttons_mask & SDL_BUTTON(SDL_BUTTON_X1);
        case MouseButton::X2:
            return buttons_mask & SDL_BUTTON(SDL_BUTTON_X2);
        }
        return false;
    }


    Input::Input()
        : keyboard_state_size(0)
        , keyboard_state(nullptr)
    {

    }

    void Input::update()
    {
        keyboard_state = SDL_GetKeyboardState(&keyboard_state_size);

        mouse.update();

    }

    bool Input::is_down(const u32 key)
    {
        return keyboard_state[key];
    }


}
