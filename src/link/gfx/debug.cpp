#include "debug.hpp"

#include <fmt/ostream.h>

#include "renderer.hpp"
#include "camera.hpp"



namespace link
{
    void Debug::init()
    {
        debug_vertices.resize(VERTICES_MAX_COUNT);
        debug_colors.resize(VERTICES_MAX_COUNT);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, debug_vertices.size() * sizeof(glm::vec3), &debug_vertices[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);

        glGenBuffers(1, &cbo);
        glBindBuffer(GL_ARRAY_BUFFER, cbo);
        glBufferData(GL_ARRAY_BUFFER, debug_colors.size() * sizeof(glm::vec3), &debug_colors[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);

        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        shader.load(vertex_shader_source, fragment_shader_source);
    }

    void Debug::draw()
    {
        if (vertex_count == 0)
        {
            return;
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(glm::vec3), &debug_vertices[0]);

        glBindBuffer(GL_ARRAY_BUFFER, cbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(glm::vec3), &debug_colors[0]);

        glDisable(GL_BLEND);

        glm::mat4 mvp = LINK_RENDERER->main_camera->projection * LINK_RENDERER->main_camera->view * glm::mat4(1);
        shader.use();
        shader.set("mvp", mvp);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_LINES, 0, vertex_count);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        glEnable(GL_BLEND);

        vertex_count = 0;
    }

    void Debug::line(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color)
    {
        line(point1, point2, color, color);
    }


    void Debug::line(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color1, const glm::vec3& color2)
    {
        if (vertex_count + 2 < VERTICES_MAX_COUNT)
        {
            debug_vertices[vertex_count + 0] = point1;
            debug_vertices[vertex_count + 1] = point2;
            debug_colors[vertex_count + 0] = color1;
            debug_colors[vertex_count + 1] = color2;

            vertex_count += 2;
        }
        else
        {
            fmt::print("WARNING :: VERTICES_MAX_COUNT reached");
        }
    }

    void Debug::cube(const glm::vec3& center, const f32 width, const f32 height, const glm::vec3& color)
    {
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;

        glm::vec3 A{ center.x - halfWidth, center.y + halfWidth, center.z - halfHeight };
        glm::vec3 B{ center.x + halfWidth, center.y + halfWidth, center.z - halfHeight };
        glm::vec3 C{ center.x + halfWidth, center.y - halfWidth, center.z - halfHeight };
        glm::vec3 D{ center.x - halfWidth, center.y - halfWidth, center.z - halfHeight };
        glm::vec3 E{ center.x - halfWidth, center.y + halfWidth, center.z + halfHeight };
        glm::vec3 F{ center.x + halfWidth, center.y + halfWidth, center.z + halfHeight };
        glm::vec3 G{ center.x + halfWidth, center.y - halfWidth, center.z + halfHeight };
        glm::vec3 H{ center.x - halfWidth, center.y - halfWidth, center.z + halfHeight };

        line(A, B, color);
        line(B, C, color);
        line(C, D, color);
        line(D, A, color);

        line(A, E, color);
        line(B, F, color);
        line(C, G, color);
        line(D, H, color);

        line(E, F, color);
        line(F, G, color);
        line(G, H, color);
        line(H, E, color);
    }
}

