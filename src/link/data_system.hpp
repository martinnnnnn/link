#pragma once


#include <vector>
#include <map>
#include <string>
#include <memory>

#include "singleton.hpp"
#include "link/gfx/texture_2d.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace link
{
    struct Mesh;
    struct Texture2D;

    struct DataSystem : Singleton<DataSystem>
    {
        struct AssimpModel
        {
            ~AssimpModel();

            bool flip_uvs;
            std::vector<std::unique_ptr<Mesh>> meshes;
            std::vector<Texture2D*> textures;
        };

        ~DataSystem();

        AssimpModel*    load_model_assimp(const std::string& path, bool flip_uvs = false);
        void            unload_model_assimp(const std::string& path);

        Texture2D*      load_texture_2d(const std::string& path, TextureType type);
        void            unload_texture_2d(const std::string& path);

        Mesh* get_sphere();
        Mesh* get_cube();
        Texture2D* get_default_texture();

    private:
        Assimp::Importer                    importer;
        std::map<std::string, std::unique_ptr<Texture2D>>   loaded_textures;
        std::map<std::string, std::unique_ptr<AssimpModel>> loaded_models;
        //std::map<std::string, Mesh*>      loaded_meshes;

        void load_material_textures(aiMaterial* mat, const std::string& directory, AssimpModel& model);
        void process_mesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, AssimpModel& model);
        void process_node(aiNode* node, const aiScene* scene, const std::string& directory, AssimpModel& model);

        static TextureType get_type_from_assimp(aiTextureType assimp_type);
    };
}

#define LINK_DATA_SYSTEM link::DataSystem::get()
