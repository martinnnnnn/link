#include "terrain.hpp"

#include <chrono>
#include <vector>
#include <fmt/ostream.h>
#include <stb_image.h>

#include "link/data_system.hpp"
#include "link/timer.hpp"
#include "link/gfx/renderer.hpp"
#include "link/gfx/camera.hpp"
#include "gfx/debug.hpp"

namespace link
{
    HeightMap::HeightMap(const glm::vec3& position)
        : vao(0)
        , vbo_vertices(0)
        , vbo_normals(0)
        , vbo_tex_coords(0)
        , ebo(0)
        , texture()
        , position(position)
        , dimensions()
    {}

    void HeightMap::init_from_simplex(const glm::uvec3& dimensions)
    {
        clear();

        this->dimensions = dimensions;

        // generate height data
        height_data.resize(dimensions.x * dimensions.z);

        for (u32 j = 0; j < dimensions.z; ++j)
        {
            for (u32 i = 0; i < dimensions.x; ++i)
            {
                simplex_noise noise(200, 200, 50, 0.2f);
                f32 height_value = noise.fractal(2, i, j);
                height_data[i + j * dimensions.z] = height_value;
            }
        }

        generate_mesh_data();
        load_mesh_gpu();
        shader_load();
    }

    void HeightMap::init_from_diamond_square(f32 y_scale)
    {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();

        std::default_random_engine engine{ (u32)seed };
        std::uniform_real_distribution<float> distribution{ 0, 1 };

        constexpr auto size = 512;
        //constexpr auto edge = size - 1;

        std::vector<std::vector<u8>> map;
        map.resize(size);
        for (auto& ys : map)
        {
            ys.resize(size, 0);
        }

        map[0][0] = 0;
        //map[0][0] = map[edge][0] = map[0][edge] = map[edge][edge] = 128;

        heightfield::diamond_square_wrap(
            size,
            // random
            [&engine, &distribution](float range)
            {
                return distribution(engine) * range;
            },
            // variance
                [](int level) -> float
            {
                return 128.0f * std::pow(0.5f, level);
            },
                // at
                [&map](int x, int y) -> u8&
            {
                return map[y][x];
            }
            );

        dimensions.x = size;
        dimensions.z = size;

        height_data.resize(dimensions.x * dimensions.z);


        for (u32 j = 0; j < dimensions.z; ++j)
        {
            for (u32 i = 0; i < dimensions.x; ++i)
            {
                height_data[i + j * dimensions.z] = (f32)map[i][j] / 255.0f * y_scale;
            }
        }

        generate_mesh_data();
        load_mesh_gpu();
        shader_load();

        texture = LINK_DATA_SYSTEM->load_texture_2d(std::string(LINK_DATA_ROOT) + "textures/pavement.jpg", TextureType::NONE);
    }

    void HeightMap::generate_mesh_data()
    {
        // reading vertices
        const f32 horizontal_scale = 1.0f;

        glm::vec3 terrain_dimensions{
            (dimensions.x - 1) * horizontal_scale,
            (dimensions.y),
            (dimensions.z - 1) * horizontal_scale
        };

        const f32 half_terrain_x_width = terrain_dimensions.x * 0.5f;
        const f32 half_terrain_z_width = terrain_dimensions.z * 0.5f;

        unsigned int vertices_count = dimensions.x * dimensions.z;
        vertices.resize(vertices_count);
        normals.resize(vertices_count);
        tex_coords.resize(vertices_count);

        for (u32 j = 0; j < dimensions.z; ++j)
        {
            for (u32 i = 0; i < dimensions.x; ++i)
            {
                unsigned int index = (j * dimensions.x) + i;
                const float heightValue = height_data[i + j * dimensions.z];

                float S = (i / (float)(dimensions.x - 1));
                float T = (j / (float)(dimensions.z - 1));

                float X = (S * terrain_dimensions.x) - half_terrain_x_width;
                float Y = heightValue * horizontal_scale;
                float Z = (T * terrain_dimensions.z) - half_terrain_z_width;

                normals[index] = glm::vec3(0);
                vertices[index] = glm::vec3(X, Y, Z);
                tex_coords[index] = glm::vec2(S * 50.f, T * 50.f);
            }
        }

        // computing indices
        const u32 triangles_count = (dimensions.x - 1) * (dimensions.z - 1) * 2;

        indices.resize((size_t)triangles_count * 3);

        unsigned int index = 0;
        for (u32 j = 0; j < (dimensions.z - 1); ++j)
        {
            for (u32 i = 0; i < (dimensions.x - 1); ++i)
            {
                int vertexIndex = (j * dimensions.x) + i;
                // Top triangle (T0)
                indices[index++] = vertexIndex;                     // V0
                indices[index++] = vertexIndex + dimensions.x + 1;  // V3
                indices[index++] = vertexIndex + 1;                 // V1
                // Bottom triangle (T1)                         
                indices[index++] = vertexIndex;                     // V0
                indices[index++] = vertexIndex + dimensions.x;      // V2
                indices[index++] = vertexIndex + dimensions.x + 1;  // V3
            }
        }

        // computing normals
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            glm::vec3 v0 = vertices[indices[i + 0]];
            glm::vec3 v1 = vertices[indices[i + 1]];
            glm::vec3 v2 = vertices[indices[i + 2]];

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            normals[indices[i + 0]] += normal;
            normals[indices[i + 1]] += normal;
            normals[indices[i + 2]] += normal;
        }

        const glm::vec3 UP(0.0f, 1.0f, 0.0f);
        for (size_t i = 0; i < normals.size(); ++i)
        {
            normals[i] = glm::normalize(normals[i]);
        }
    }

    void HeightMap::load_mesh_gpu()
    {
        clear();

        glGenBuffers(1, &vbo_normals);
        glGenBuffers(1, &ebo);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &vbo_tex_coords);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_tex_coords);
        glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(glm::vec2), tex_coords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void HeightMap::shader_load()
    {
        std::string vspath = LINK_DATA_ROOT; vspath += "glsl/height_map.vs";
        std::string fspath = LINK_DATA_ROOT; fspath += "glsl/height_map.fs";

        shader.load(vspath, fspath);
        shader.use();
        shader.set("tex_sampler", 0);
    }

    void HeightMap::clear()
    {
        if (vao != 0)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        if (vbo_vertices != 0)
        {
            glDeleteBuffers(1, &vbo_vertices);
            vbo_vertices = 0;
        }

        if (vbo_normals != 0)
        {
            glDeleteBuffers(1, &vbo_normals);
            vbo_normals = 0;
        }

        if (vbo_tex_coords != 0)
        {
            glDeleteBuffers(1, &vbo_tex_coords);
            vbo_tex_coords = 0;
        }

        if (ebo != 0)
        {
            glDeleteBuffers(1, &ebo);
            ebo = 0;
        }
    }

    namespace
    {
        glm::vec3 lightPos = { 0, 200.0f, 0 };
        glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
        bool forward = true;
    }

    void HeightMap::draw()
    {
        if (forward)
        {
            lightPos.x += 0.1f * LINK_TIME->timer.dt_ms;
            lightPos.z += 0.1f * LINK_TIME->timer.dt_ms;

            if (lightPos.x > 512)
            {
                forward = !forward;
            }
        }
        else
        {
            lightPos.x -= 0.1f * LINK_TIME->timer.dt_ms;
            lightPos.z -= 0.1f * LINK_TIME->timer.dt_ms;

            if (lightPos.x < -512)
            {
                forward = !forward;
            }
        }

        //for (u32 i = 0; i < normals.size(); ++i)
        //{
        //    LINK_DEBUG->line(vertices[i], vertices[i] + normals[i] * 3.0f, {1.0f, 0, 0});
        //}

        //texture->bind(GL_TEXTURE0);

        shader.use();
        shader.set("lightPos", lightPos);
        shader.set("lightColor", lightColor);
        shader.set("model", glm::translate(glm::mat4(1), position));
        shader.set("projection", LINK_RENDERER->main_camera->projection);
        shader.set("view", LINK_RENDERER->main_camera->view);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}