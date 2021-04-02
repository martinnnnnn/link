#include <GL/glew.h>
#include <gl/GL.h>
// SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>

#include <cstdio>
#include <thread>
#include <iostream>
#include <vector>
#include <memory>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fmt/ostream.h>

#include <json.hpp>

#include "types.hpp"
#include "file.hpp"
#include "file_system.hpp"
#include "timer.hpp"
#include "window.hpp"
#include "editor/editor.hpp"
#include "gfx/debug.hpp"
#include "random.hpp"
#include "input.hpp"
#include "terrain.hpp"
#include "diamond_square.hpp"
#include "gfx/shader.hpp"
#include "gfx/camera.hpp"
#include "gfx/framebuffer.hpp"
#include "gfx/texture_2d.hpp"
#include "gfx/mesh.hpp"
#include "gfx/mesh.hpp"
#include "gfx/renderer.hpp"
#include "scene/scene.hpp"
#include "scene/scene_object.hpp"
#include "scene/game.hpp"
#include "scene/components/c_transform.hpp"
#include "scene/components/c_material.hpp"
#include "scene/components/c_material_pbr.hpp"
#include "scene/components/c_material_phong.hpp"
#include "scene/components/c_model_static.hpp"
#include "scene/components/c_light_source.hpp"
#include "voxel/volume_data.hpp"
#include "voxel/surface_extractor.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>

using namespace link;

FileSystem file_system;

#ifdef LINK_EDITOR_ENABLED
void debug_draw();
#else
void debug_draw() {}
#endif


int main()
{
    LINK_TIME->start();
    CRandom::initialize();

    file_system.init(LINK_DATA_ROOT);

    LINK_WINDOW->init({ 1980, 1080 });
    LINK_EDITOR->init();
    LINK_DEBUG->init();

    LINK_GAME->init(/*std::string(LINK_DATA_ROOT) + "scenes/"*/"");

#ifdef LINK_EDITOR_ENABLED
    LINK_RENDERER->to_framebuffer({ 1980, 1080 });
#else
    LINK_GAME->play();
    LINK_RENDERER->to_viewport({ 1980, 1080 });
#endif

    //HeightMap height_map(glm::vec3(0, 0, 0));
    //height_map.init_from_diamond_square(200);
    //height_map.init_from_simplex({ 512, 512, 512 });

    Shader shader;
    shader.load(std::string(LINK_DATA_ROOT) + "glsl/static.vs", std::string(LINK_DATA_ROOT) + "glsl/solid_color.fs");
    shader.use();
    shader.bind_ub("Camera", BindingPoint::CAMERA);

    VolumeData32* data = new VolumeData32();

    for (u64 i = 0; i < VolumeSize<DEFAULT_DATA_SIZE>::cubed; ++i)
    {
        SurfaceExtractor::transvoxel(data, data->chunks[i].get());
    }

    bool done = false;
    while (!done)
    {
        LINK_TIME->update();
        LINK_INPUT->update();
        LINK_INPUT->mouse.wheel = MouseWheel::NONE;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            LINK_EDITOR->process_events(&event);

            switch (event.type)
            {
            case SDL_QUIT:
                {
                    done = true;
                    break;
                }
            case SDL_WINDOWEVENT:
                {
                    switch (event.window.event)
                    {
                    case SDL_WINDOWEVENT_CLOSE:
                    {
                        if (event.window.windowID == SDL_GetWindowID(LINK_WINDOW->window))
                        {
                            done = true;
                        }
                        break;
                    }
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        LINK_WINDOW->on_resize({ event.window.data1, event.window.data2 });
                        break;
                    }
                    case SDL_WINDOWEVENT_MOVED:
                    {
                        LINK_WINDOW->on_move({ event.window.data1, event.window.data2 });
                        break;
                    }
                    }
                    break;
                }
            case SDL_MOUSEWHEEL:
                {
                    if (event.wheel.y != 0)
                    {
                        LINK_INPUT->mouse.wheel = MouseWheel(event.wheel.y);
                    }
                    break;
                }
            }
        }

        if (LINK_INPUT->is_down(SDL_SCANCODE_ESCAPE))
        {
            done = true;
        }

        if (LINK_INPUT->mouse.down(MouseButton::RIGHT))
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            LINK_RENDERER->main_camera->update();
        }
        else
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        LINK_GAME->update();
        LINK_GAME->debug_update();

        LINK_EDITOR->begin_frame();

        LINK_GAME->debug_draw();

        debug_draw();

        // ---------------------------------
        // DRAWING
        LINK_RENDERER->bind();

        LINK_DEBUG->cube(glm::vec3(-5, -5, 0), 5, 5, glm::vec3(1, 0, 0));
        LINK_DEBUG->draw();

        for (u64 i = 0; i < VolumeSize<DEFAULT_DATA_SIZE>::cubed; ++i)
        {
            shader.use();
            shader.set("color", glm::vec3{ 1.0, 0, 0 });
            shader.set("model", glm::translate(glm::mat4(1.0f), glm::vec3(data->chunks[i]->position) * 32.f));
            data->chunks[i]->draw();
        }

        //height_map.draw();

        LINK_RENDERER->draw();

        LINK_RENDERER->unbind();
        // ---------------------------------

        LINK_EDITOR->end_frame();

        SDL_GL_SwapWindow(LINK_WINDOW->window);
    }

    LINK_EDITOR->shutdown();
    LINK_WINDOW->shutdown();

    return 0;
}


#ifdef LINK_EDITOR_ENABLED
void debug_draw()
{
    static EString scene_create_name("Scene Name");

    static bool demo_open = false;

    if (ImGui::Begin("Data Explorer"))
    {
        file_system.debug_draw();
        ImGui::End();
    }

    ImGui::Begin(Editor::MAIN_WINDOW);
    if (ImGui::BeginMenuBar())
    {
        if (EUtils::Button("MainMenu", "Open Demo"))
        {
            demo_open = !demo_open;
        }
        if (ImGui::BeginMenu("Game"))
        {
            if (ImGui::Button("New Scene"))
            {
                ImGui::OpenPopup("Scene Creation");
            }

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Scene Creation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                scene_create_name.debug_draw();

                if (EUtils::Button("SceneCreation", "Create", { 0.5, 0, 0 }, { 120, 0 }))
                {
                    LINK_GAME->create_scene(scene_create_name.value);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (EUtils::Button("SceneCreation", "Cancel", { 0.5, 0, 0 }, { 120, 0 }))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

    if (demo_open) ImGui::ShowDemoWindow(&demo_open);
}
#endif