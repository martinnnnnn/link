#include "game.hpp"

#include <filesystem>
#include <fmt/ostream.h>
#include <imgui.h>

#include "link/string_utils.hpp"
#include "scene.hpp"
#include "link/file_system.hpp"
#include "link/physics/physics.hpp"

namespace link
{
    Game::~Game() = default;

    Game::Game()
    {
    }

    void Game::create_scene(const std::string& name)
    {
        std::string scene_name = name;

        bool rename = false;
        int index = 1;
        do {
            for (auto& scene : scenes)
            {
                if (scene->name.value.compare(scene_name) == 0)
                {
                    scene_name = name + std::to_string(index++);
                    rename = true;
                }
            }
        } while (rename);

        scenes.emplace_back(std::make_unique<Scene>(name));
    }

    void Game::init(const std::string& p)
    {
        if (p.empty())
        {
            return;
        }

        path = p;

        for (const std::filesystem::directory_entry& p : std::filesystem::directory_iterator(path))
        {
            // clean path
            std::string path = p.path().string();
            string::replace(path, "\\", "/");

            std::string_view ext = Path::get_extention(path);

            if (ext.compare("link") == 0)
            {
                fmt::print("Loading scene {}\n", path);
                scenes.emplace_back(std::make_unique<Scene>(path, true));
            }

        }
    }

    void Game::play()
    {
        if (state == State::Stoped)
        {
            for (auto& scene : scenes)
            {
                scene->save();
                scene->init();
            }
            state = State::Playing;
        }
        if (state == State::Paused)
        {
            state = State::Playing;
        }
    }

    void Game::update()
    {
        if (state == State::Playing)
        {
            for (auto& scene : scenes)
            {
                scene->update();
            }
            LINK_PHYSICS->update();
        }
    }

    void Game::pause()
    {
        if (state == State::Playing)
        {
            state = State::Paused;
        }
    }

    void Game::stop()
    {
        if (state == State::Playing || state == State::Paused)
        {
            state = State::Stoped;
            for (auto& scene : scenes)
            {
                scene->stop();
                scene->load();
            }
        }
    }

#ifdef LINK_EDITOR_ENABLED
    void Game::debug_update()
    {
        if (state == State::Stoped)
        {
            for (auto& scene : scenes)
            {
                scene->debug_update();
            }
        }
    }

    void Game::debug_draw()
    {
        if (ImGui::Begin(Editor::MAIN_WINDOW))
        {
            if (ImGui::BeginMenuBar())
            {
                if ((state == State::Stoped || state == State::Paused) && EUtils::Button("Game", "Play", { 0, 1, 0 }, { 120, 0 }))
                {
                    play();
                }
                if (state == State::Playing && EUtils::Button("Game", "Pause", { 1, 1, 0 }, { 120, 0 }))
                {
                    pause();
                }
                if ((state == State::Playing || state == State::Paused) && EUtils::Button("Game", "Stop", { 1, 0, 0 }, { 120, 0 }))
                {
                    stop();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        for (auto& scene : scenes)
        {
            scene->debug_draw();
        }
    }
#endif
}