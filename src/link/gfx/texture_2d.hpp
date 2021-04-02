#pragma once


#include <GL/glew.h>
#include <gl/GL.h>

#include <string>
#include <memory>
#include <map>
#include <array>

#include "link/types.hpp"
#include "link/singleton.hpp"

namespace link
{
    enum class TextureType
    {
        NONE = 0,

        DIFFUSE = 1,
        SPECULAR = 2,
        AMBIENT = 3,
        EMISSIVE = 4,
        HEIGHT = 5,
        NORMAL = 6,
        SHININESS = 7,
        OPACITY = 8,
        DISPLACEMENT = 9,
        LIGHTMAP = 10,
        REFLECTION = 11,

        // PRB
        PBR_ALBEDO = 12,            // aiTextureType_BASE_COLOR
        PBR_NORMAL = 13,            // aiTextureType_NORMAL_CAMERA
        PBR_EMISSIVE = 14,          // aiTextureType_EMISSION_COLOR
        PBR_METALLIC = 15,          // aiTextureType_METALNESS
        PBR_ROUGHNESS = 16,         // aiTextureType_DIFFUSE_ROUGHNESS
        PBR_AMBIENT_OCCLUSION = 17, // aiTextureType_AMBIENT_OCCLUSION

        COUNT = 18,
    };

    struct Texture2D
    {
        Texture2D();

        Texture2D(const std::string& file_path, TextureType type, bool generate_mipmaps = true);
        bool load(const std::string& file_path, TextureType type, bool generate_mipmaps = true);

        ~Texture2D();

        void bind(int texture_unit) const;
        void clear();

        GLuint id;
        i32 width;
        i32 height;
        i32 bytes_per_pixel;
        GLenum format;
        bool generate_mipmaps;
        std::string file_path;
        TextureType type;
    };
}
