#include  "surface_extractor.hpp"

#include <fmt/ostream.h>

#include "link/gfx/mesh.hpp"
#include "transvoxel_tables.hpp"

namespace link
{
    namespace
    {
        constexpr glm::ivec3 CORNER_INDEXES[]
        {
            glm::ivec3(0, 0, 0),
            glm::ivec3(1, 0, 0),
            glm::ivec3(0, 0, 1),
            glm::ivec3(1, 0, 1),
            glm::ivec3(0, 1, 0),
            glm::ivec3(1, 1, 0),
            glm::ivec3(0, 1, 1),
            glm::ivec3(1, 1, 1)
        };

        u8 get_case_code(Sample* density[8])
        {
            u8 code = 0;
            u8 konj = 0x01;
            for (i32 i = 0; i < 8; i++)
            {
                code |= u8((density[i]->value >> (8 - 1 - i)) & konj);
                konj <<= 1;
            }

            return code;
        }

        void polygonize_cell(VolumeData32* data, const glm::ivec3& chunk_position, const glm::ivec3& sample_position, std::vector<Vertex>& vertices, std::vector<u32>& indices)
        {
            const glm::ivec3 absolute_position = chunk_position + sample_position;

            Sample* cell[8];

            for (i64 i = 0; i < 8; i++)
            {
                cell[i] = data->sample(absolute_position + CORNER_INDEXES[i]);
            }

            u8 case_code = get_case_code(cell);

            if ((case_code ^ ((cell[7]->value >> 7) & 0xFF)) == 0)
            {
                return;
            }

            glm::vec3 corner_normals[8];
            for (i32 i = 0; i < 8; i++)
            {
                glm::ivec3 p = absolute_position + CORNER_INDEXES[i];

                
                const glm::vec3 normal =
                {
                    f32(data->sample(p + glm::ivec3(1, 0, 0))->value - data->sample(p - glm::ivec3(1, 0, 0))->value) * 0.5f,
                    f32(data->sample(p + glm::ivec3(0, 1, 0))->value - data->sample(p - glm::ivec3(0, 1, 0))->value) * 0.5f,
                    f32(data->sample(p + glm::ivec3(0, 0, 1))->value - data->sample(p - glm::ivec3(0, 0, 1))->value) * 0.5f
                };

                corner_normals[i] = glm::normalize(normal);
            }

            u8 reg_cell_class = lengyel::regularCellClass[case_code];
            const u16* vertex_locations = lengyel::regularVertexData[case_code];

            lengyel::RegularCellData c = lengyel::regularCellData[reg_cell_class];
            long vertex_count = c.GetVertexCount();
            long triangle_count = c.GetTriangleCount();

            std::vector<u32> added_indices;
            added_indices.resize(c.GetTriangleCount() * 3);

            for (int i = 0; i < vertex_count; i++)
            {
                u8 edge = u8(vertex_locations[i] >> 8);
                u8 reuse_index = u8(edge & 0xF); //Vertex id which should be created or reused 1,2 or 3
                u8 reach_direction = u8(edge >> 4); //the direction to go to reach a previous cell for reusing 

                u8 v1 = u8((vertex_locations[i]) & 0x0F); //Second Corner Index
                u8 v0 = u8((vertex_locations[i] >> 4) & 0x0F); //First Corner Index

                Sample* d0 = cell[v0];
                Sample* d1 = cell[v1];

                const f32 t = f32(d1->value) / f32(d1->value - d0->value);

                const glm::vec3 P0 = (absolute_position + CORNER_INDEXES[v0]);
                const glm::vec3 P1 = (absolute_position + CORNER_INDEXES[v1]);

                const glm::vec3 position = P0 * t + P1 * (1.0f - t);
                const glm::vec3 normal = corner_normals[v0] * t + corner_normals[v1] * (1.0f - t);

                vertices.emplace_back(position, glm::vec2{}, normal);
                added_indices[i] = u32(vertices.size() - 1);
            }

            std::reverse(c.vertexIndex, c.vertexIndex + (c.GetTriangleCount() * 3));

            for (int t = 0; t < triangle_count; t++)
            {
                for (int i = 0; i < 3; i++)
                {
                    indices.push_back(added_indices[c.vertexIndex[t * 3 + i]]);
                }
            }

            //if (chunk->mesh)
            //{
            //    chunk->mesh->vertices = std::move(vertices);
            //    chunk->mesh->indices = std::move(indices2);
            //    chunk->mesh->data_updated();
            //}
            //else
            //{
            //    fmt::print("computed a mesh with {} vertices and {} indices\n", vertices.size(), indices2.size());
            //    chunk->mesh = std::make_unique<Mesh>(vertices, indices2);
            //}
        }

    }

    void SurfaceExtractor::transvoxel(VolumeData32* data, VolumeChunk32* chunk)
    {
        if (chunk->mesh)
        {
            chunk->mesh->vertices.clear();
            chunk->mesh->indices.clear();
        }
        else
        {
            chunk->mesh = std::make_unique<Mesh>(std::vector<Vertex>(), std::vector<u32>());
        }

        for (i32 x = 0; x < VolumeSize32::value; x++)
        {
            for (i32 y = 0; y < VolumeSize32::value; y++)
            {
                for (i32 z = 0; z < VolumeSize32::value; z++)
                {
                    polygonize_cell(data, chunk->position, { x, y, z }, chunk->mesh->vertices, chunk->mesh->indices);
                }
            }
        }

        fmt::print("computed a mesh with {} vertices and {} indices\n", chunk->mesh->vertices.size(), chunk->mesh->indices.size());
        chunk->mesh->data_updated();
    }

}