#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


#include "link/types.hpp"
#include "link/gfx/mesh.hpp"

#include <GL/glew.h>
#include <gl/GL.h>

namespace link::experimental
{
    using namespace link;

    struct Mesh_V2
    {
        enum class DataMode { CLASSIC, INDEXED };

        u32 VAO, VBO, EBO;
        GLenum draw_mode;
        DataMode data_mode;

        u32 vertices_count;
        u32 max_vertices_count;

        u32 indices_count;
        u32 max_indices_count;

        Mesh_V2(const DataMode data_mode = DataMode::INDEXED, const GLenum draw_mode = GL_TRIANGLES)
            : data_mode(data_mode)
            , draw_mode(draw_mode)
            , VAO(0)
            , VBO(0)
            , EBO(0)
            , vertices_count(0)
            , max_vertices_count(0)
            , indices_count(0)
            , max_indices_count(0)
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            if (data_mode == DataMode::INDEXED)
            {
                glGenBuffers(1, &EBO);
            }
        }

        ~Mesh_V2()
        {
            glDeleteVertexArrays(1, &VAO);
        }

        void init_indexed(const u32 max_vertices, const u32 max_indices)
        {
            assert(data_mode == DataMode::INDEXED && EBO != 0);

            max_vertices_count = max_vertices;
            max_indices_count = max_indices;

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(Vertex), NULL, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), NULL, GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex texture coords
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            // vertex normals
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            glBindVertexArray(0);
        }

        void update_data_indexed(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
        {
            assert(data_mode == DataMode::INDEXED && EBO != 0);
            assert(vertices.size() < max_vertices_count);
            assert(indices.size() < max_indices_count);

            vertices_count = vertices.size();
            indices_count = indices.size();

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(u32), &indices[0]);
        }

        void draw()
        {
            glBindVertexArray(VAO);
            glDrawElements(draw_mode, indices_count, GL_UNSIGNED_INT, 0);
            glBindVertexArray(VAO);
        }
    };


    struct VolumeSize
    {
        VolumeSize(i32 size)
            : value(size)
            , squared(size * size)
            , cubed(size * size * size)
        {}

        const i32 value;
        const i32 squared;
        const i32 cubed;

        inline i32          get_offset(const i32 x, const i32 y, const i32 z) { return x + y * value + z * squared; }
        inline i32          get_offset(const glm::ivec3& position) { return get_offset(position.x, position.y, position.z); }
        inline glm::ivec3   get_position(i32 offset)
        {
            const i32 z = offset / squared;
            const i32 y = (offset - z * squared) / value;
            const i32 x = (offset - z * squared - y * value);

            return  glm::ivec3{ x, y, z };
        }
    };

    struct VoxelChunk
    {

    };

    struct VoxelData
    {
        std::unordered_map<glm::ivec3, VoxelChunk> chunks;
    };
}


