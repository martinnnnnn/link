#pragma once

#include <GL/glew.h>
#include <gl/GL.h>

#include <glm/glm.hpp>

#include <assimp/scene.h>

#include <vector>
#include <string>

#include "link/types.hpp"

namespace link
{
    struct Vertex
    {
        Vertex() {}
        Vertex(const glm::vec3& position, const glm::vec2& tex_coords, const glm::vec3& normal) : Position(position), TexCoords(tex_coords), Normal(normal) {}

        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
    };


    struct Mesh
    {
        u32 VAO, VBO, EBO;
        GLenum mode;

        std::vector<Vertex> vertices;
        std::vector<u32> indices;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, GLenum mode = GL_TRIANGLES);
        Mesh(const std::vector<Vertex>& vertices, GLenum mode = GL_TRIANGLES);

        void data_updated();

        ~Mesh();
        void draw();
    };
}
