#pragma once

#include <vector>
#include <math.h>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.hpp"
#include "gfx/shader.hpp"
#include "gfx/texture_2d.hpp"
#include "random.hpp"
#include "simplex_noise.hpp"
#include "diamond_square.hpp"


namespace link
{
    struct HeightMap
    {
        HeightMap(const glm::vec3& position);

        std::vector <glm::vec3> vertices;
        std::vector <glm::vec3> normals;
        std::vector <glm::vec2> tex_coords;
        std::vector <u32> indices;

        Shader shader;

        GLuint vao;
        GLuint vbo_vertices;
        GLuint vbo_normals;
        GLuint vbo_tex_coords;
        GLuint ebo;

        Texture2D* texture;

        glm::vec3 position;
        glm::uvec3 dimensions;
        std::vector<f32> height_data;


        void init_from_file(const std::string& file_path) {}
        void init_from_diamond_square(f32 y_scale);
        void init_from_simplex(const glm::uvec3& dimensions);

        void clear();
        void draw(/*const glm::mat4& projection, const glm::mat4& view*/);

        void generate_mesh_data();
        void load_mesh_gpu();
        void shader_load();
    };
}