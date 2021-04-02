#pragma once

#include <vector>
#include <memory>
#include <string>

#include "link/singleton.hpp"
#include "link/editor/editor.hpp"

namespace link
{
    struct Scene;

    struct Game : Singleton<Game>
    {
        enum class State
        {
            Playing,
            Paused,
            Stoped
        };

        std::vector<std::unique_ptr<Scene>> scenes;
        State state = State::Stoped;
        std::string path;

        Game();
        ~Game();

        void create_scene(const std::string& name);

        void init(const std::string& path);

        void play();
        void update();
        void pause();
        void stop();

#ifdef LINK_EDITOR_ENABLED
        void debug_update();
        void debug_draw();
#else
        void debug_update() {}
        void debug_draw() {}
#endif
    };
}

#define LINK_GAME link::Game::get()