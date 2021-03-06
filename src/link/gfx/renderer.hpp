#pragma once


#include <set>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/GL.h>

#include "link/singleton.hpp"
#include "framebuffer.hpp"
#include "ubo.hpp"

namespace link
{
    struct CModel_Static;
    struct CLightSource;
    struct Camera;

    struct Renderer : Singleton<Renderer>
    {
        Renderer();

        void subscribe(CModel_Static* model);
        void unsubscribe(CModel_Static* model);

        void subscribe(CLightSource* light_source);
        void unsubscribe(CLightSource* light_source);

        void draw();

        void bind();
        void unbind();

        void to_framebuffer(const glm::vec2& size);
        void to_viewport(const glm::vec2& size);
        void imgui_swap();
        void update_framebuffer_size(const glm::vec2& size);

        std::set<CModel_Static*> models_static;

        std::set<CLightSource*> light_sources;
        UBO light_sources_ubo;
        static constexpr u32 MAX_LIGHTS_SOURCES = 20;

        Camera* main_camera;
        FrameBuffer framebuffer;
        glm::ivec2 editor_position;
        glm::vec2 viewport_size;
    };
}

#define LINK_RENDERER link::Renderer::get()
