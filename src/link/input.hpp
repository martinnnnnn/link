#pragma once

#include <glm/glm.hpp>

#include "singleton.hpp"
#include "types.hpp"
#include "physics/shapes.hpp"

namespace link
{
    enum class MouseWheel
    {
        UP = 1,
        DOWN = -1,
        NONE = 0
    };

    enum class MouseButton
    {
        LEFT,
        MIDDLE,
        RIGHT,
        X1,
        X2
    };

    struct Mouse
    {
        Mouse();

        void update();
        bool down(MouseButton button);

        glm::ivec2 position;
        glm::ivec2 offset;
        u32 buttons_mask;
        MouseWheel wheel;


    };

    struct Input : Singleton<Input>
    {
        Input();

        void update();

        bool is_down(const u32 key);

        i32			keyboard_state_size;
        const u8* keyboard_state;

        Mouse		mouse;

        //i32			mouse_offset_x;
        //i32			mouse_offset_y;

        //MouseWheel	mouse_wheel;
    };
}

#define LINK_INPUT link::Input::get()
