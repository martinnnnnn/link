#include "data_system.hpp"

#include <assert.h>
#include <assimp/postprocess.h>
#include <fmt/ostream.h>

#include "gfx/mesh.hpp"

namespace link
{
    DataSystem::~DataSystem() = default;
    DataSystem::AssimpModel::~AssimpModel() = default;

    DataSystem::AssimpModel* DataSystem::load_model_assimp(const std::string& path, bool flip_uvs)
    {
        AssimpModel* model = nullptr;

        auto iter(loaded_models.lower_bound(path));

        if (iter == loaded_models.end() || path < iter->first)
        {
            //model = new AssimpModel();
            model = loaded_models.insert(iter, std::make_pair(path, std::make_unique<AssimpModel>()))->second.get();
            model->flip_uvs = flip_uvs;

            u32 import_flags = aiProcess_Triangulate;
            if (flip_uvs)
            {
                import_flags |= aiProcess_FlipUVs;
            }
            const aiScene* scene = importer.ReadFile(path, import_flags);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                fmt::print("ERROR::ASSIMP:: {}\n", importer.GetErrorString());
                return nullptr;
            }
            std::string directory = path.substr(0, path.find_last_of('/') + 1);

            process_node(scene->mRootNode, scene, directory, *model);
            //loaded_models.insert(iter, std::make_pair(path, model));
        }
        else if (iter->second->flip_uvs != flip_uvs)
        {
            unload_model_assimp(path);
            model = load_model_assimp(path, flip_uvs);
        }
        else
        {
            model = iter->second.get();
        }

        if (model)
        {
            fmt::print("Loaded model : {}\n", path);
        }

        return model;
    }

    void DataSystem::unload_model_assimp(const std::string& path)
    {
        loaded_models.erase(loaded_models.find(path));
    }

    Texture2D* DataSystem::load_texture_2d(const std::string& path, TextureType type)
    {
        Texture2D* texture = nullptr;

        auto iter(loaded_textures.lower_bound(path));

        if (iter == loaded_textures.end() || path < iter->first)
        {
            fmt::print("Attempting to load {}\n", path);
            //texture = new Texture2D(path, type);
            texture = loaded_textures.insert(iter, std::make_pair(path, std::make_unique<Texture2D>(path, type)))->second.get();
        }
        else
        {
            texture = iter->second.get();
        }

        return texture;
    }

    void DataSystem::unload_texture_2d(const std::string& path)
    {
        loaded_textures.erase(loaded_textures.find(path));
        //auto iter(loaded_textures.find(path));
        //if (iter != loaded_textures.end())
        //{
        //    iter->second->clear();
        //    delete iter->second;
        //    loaded_textures.erase(iter);
        //}
    }

    void DataSystem::process_node(aiNode* node, const aiScene* scene, const std::string& directory, AssimpModel& model)
    {
        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            process_mesh(mesh, scene, directory, model);
            //model.meshes.emplace_back();
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            process_node(node->mChildren[i], scene, directory, model);
        }
    }

    void DataSystem::process_mesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, AssimpModel& model)
    {
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
        std::vector<Texture2D> textures;

        for (u32 i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.Position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normal = glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (u32 i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (u32 j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        model.meshes.push_back(std::make_unique<Mesh>(vertices, indices));

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            load_material_textures(material, directory, model);
        }
    }

    void DataSystem::load_material_textures(aiMaterial* mat, const std::string& directory, AssimpModel& model)
    {
        for (u32 i = 0; i <= aiTextureType_UNKNOWN; i++)
        {
            for (u32 j = 0; j < mat->GetTextureCount((aiTextureType)i); j++)
            {
                aiString str;
                mat->GetTexture((aiTextureType)i, j, &str);

                bool skip = false;

                std::string texture_path = directory + str.C_Str();

                Texture2D* texture = load_texture_2d(texture_path, get_type_from_assimp((aiTextureType)i));
                model.textures.push_back(texture);
            }
        }
    }

    Mesh* DataSystem::get_sphere()
    {
        static Mesh* sphere_mesh = nullptr;

        if (!sphere_mesh)
        {
            std::vector<Vertex> vertices;
            std::vector<u32> indices;

            const unsigned int X_SEGMENTS = 64;
            const unsigned int Y_SEGMENTS = 64;
            const float PI = 3.14159265359;
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    Vertex vertex;
                    float xSegment = (float)x / (float)X_SEGMENTS;
                    float ySegment = (float)y / (float)Y_SEGMENTS;
                    float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                    float yPos = std::cos(ySegment * PI);
                    float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                    vertex.Position = glm::vec3(xPos, yPos, zPos);
                    vertex.TexCoords = glm::vec2(xSegment, ySegment);
                    vertex.Normal = glm::vec3(xPos, yPos, zPos);
                    vertices.push_back(vertex);
                }
            }

            bool oddRow = false;
            for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
            {
                if (!oddRow) // even rows: y == 0, y == 2; and so on
                {
                    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                    {
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    }
                }
                else
                {
                    for (int x = X_SEGMENTS; x >= 0; --x)
                    {
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                    }
                }
                oddRow = !oddRow;
            }

            sphere_mesh = new Mesh(vertices, indices, GL_TRIANGLE_STRIP);
        }
        return sphere_mesh;
    }

    Mesh* DataSystem::get_cube()
    {
        static Mesh* cube_mesh = nullptr;

        if (!cube_mesh)
        {
            std::vector<Vertex> vertices =
            {
                // positions              // uvs           // normals         
                { {-0.5f, -0.5f, -0.5f},  {0.0f,  0.0f},  {0.0f,  0.0f, -1.0f }},
                { { 0.5f, -0.5f, -0.5f},  {1.0f,  0.0f},  {0.0f,  0.0f, -1.0f }},
                { { 0.5f,  0.5f, -0.5f},  {1.0f,  1.0f},  {0.0f,  0.0f, -1.0f }},
                { { 0.5f,  0.5f, -0.5f},  {1.0f,  1.0f},  {0.0f,  0.0f, -1.0f }},
                { {-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f},  {0.0f,  0.0f, -1.0f }},
                { {-0.5f, -0.5f, -0.5f},  {0.0f,  0.0f},  {0.0f,  0.0f, -1.0f }},

                { {-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f},  {0.0f,  0.0f,  1.0f }},
                { { 0.5f, -0.5f,  0.5f},  {1.0f,  0.0f},  {0.0f,  0.0f,  1.0f }},
                { { 0.5f,  0.5f,  0.5f},  {1.0f,  1.0f},  {0.0f,  0.0f,  1.0f }},
                { { 0.5f,  0.5f,  0.5f},  {1.0f,  1.0f},  {0.0f,  0.0f,  1.0f }},
                { {-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f},  {0.0f,  0.0f,  1.0f }},
                { {-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f},  {0.0f,  0.0f,  1.0f }},

                { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},
                { {-0.5f,  0.5f, -0.5f}, {-1.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { {-0.5f, -0.5f, -0.5f}, {-0.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { {-0.5f, -0.5f, -0.5f}, {-0.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { {-0.5f, -0.5f,  0.5f}, {-0.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},
                { {-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},

                { { 0.5f,  0.5f,  0.5f},  {1.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},
                { { 0.5f,  0.5f, -0.5f},  {1.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { { 0.5f, -0.5f, -0.5f},  {0.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { { 0.5f, -0.5f, -0.5f},  {0.0f,  1.0f},  {1.0f,  0.0f,  0.0f }},
                { { 0.5f, -0.5f,  0.5f},  {0.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},
                { { 0.5f,  0.5f,  0.5f},  {1.0f,  0.0f},  {1.0f,  0.0f,  0.0f }},

                { {-0.5f, -0.5f, -0.5f},  {0.0f,  1.0f},  {0.0f, -1.0f,  0.0f }},
                { { 0.5f, -0.5f, -0.5f},  {1.0f,  1.0f},  {0.0f, -1.0f,  0.0f }},
                { { 0.5f, -0.5f,  0.5f},  {1.0f,  0.0f},  {0.0f, -1.0f,  0.0f }},
                { { 0.5f, -0.5f,  0.5f},  {1.0f,  0.0f},  {0.0f, -1.0f,  0.0f }},
                { {-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f},  {0.0f, -1.0f,  0.0f }},
                { {-0.5f, -0.5f, -0.5f},  {0.0f,  1.0f},  {0.0f, -1.0f,  0.0f }},

                { {-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f},  {0.0f,  1.0f,  0.0f }},
                { { 0.5f,  0.5f, -0.5f},  {1.0f,  1.0f},  {0.0f,  1.0f,  0.0f }},
                { { 0.5f,  0.5f,  0.5f},  {1.0f,  0.0f},  {0.0f,  1.0f,  0.0f }},
                { { 0.5f,  0.5f,  0.5f},  {1.0f,  0.0f},  {0.0f,  1.0f,  0.0f }},
                { {-0.5f,  0.5f,  0.5f},  {0.0f,  0.0f},  {0.0f,  1.0f,  0.0f }},
                { {-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f},  {0.0f,  1.0f,  0.0f }},
            };

            cube_mesh = new Mesh(vertices);
        }

        return cube_mesh;
    }

    Texture2D* DataSystem::get_default_texture()
    {
        static Texture2D* texture = nullptr;

        if (!texture)
        {
            texture = new Texture2D();

            glGenTextures(1, &texture->id);

            GLubyte data[] = { 128, 128, 128, 255 };

            glBindTexture(GL_TEXTURE_2D, texture->id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            texture->width = 1;
            texture->height = 1;
        }

        return texture;
    }


    TextureType DataSystem::get_type_from_assimp(aiTextureType assimp_type)
    {
        static bool init = false;
        static TextureType tex_map[(u64)TextureType::COUNT];
        if (!init)
        {
            init = true;
            tex_map[aiTextureType_NONE] = TextureType::NONE;
            tex_map[aiTextureType_DIFFUSE] = TextureType::DIFFUSE;
            tex_map[aiTextureType_SPECULAR] = TextureType::SPECULAR;
            tex_map[aiTextureType_AMBIENT] = TextureType::AMBIENT;
            tex_map[aiTextureType_EMISSIVE] = TextureType::EMISSIVE;
            tex_map[aiTextureType_HEIGHT] = TextureType::HEIGHT;
            tex_map[aiTextureType_NORMALS] = TextureType::NORMAL;
            tex_map[aiTextureType_OPACITY] = TextureType::OPACITY;
            tex_map[aiTextureType_DISPLACEMENT] = TextureType::DISPLACEMENT;
            tex_map[aiTextureType_LIGHTMAP] = TextureType::LIGHTMAP;
            tex_map[aiTextureType_REFLECTION] = TextureType::REFLECTION;
            tex_map[aiTextureType_SHININESS] = TextureType::SHININESS;

            tex_map[aiTextureType_BASE_COLOR] = TextureType::PBR_ALBEDO;
            tex_map[aiTextureType_NORMAL_CAMERA] = TextureType::PBR_NORMAL;
            tex_map[aiTextureType_EMISSION_COLOR] = TextureType::PBR_EMISSIVE;
            tex_map[aiTextureType_METALNESS] = TextureType::PBR_METALLIC;
            tex_map[aiTextureType_DIFFUSE_ROUGHNESS] = TextureType::PBR_ROUGHNESS;
            tex_map[aiTextureType_AMBIENT_OCCLUSION] = TextureType::PBR_AMBIENT_OCCLUSION;
        }

        if (assimp_type <= aiTextureType_AMBIENT_OCCLUSION)
        {
            return tex_map[assimp_type];
        }
        return TextureType::NONE;
    }
}
