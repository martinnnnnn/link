#include "texture_2d.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fmt/ostream.h>

namespace link
{
    namespace
    {
        void init_stbi()
        {
            static bool first_load = true;
            if (first_load)
            {
                first_load = false;
                stbi_set_flip_vertically_on_load(true);
            }
        }
    }


    Texture2D::Texture2D()
        : id(0)
        , width(0)
        , height(0)
        , bytes_per_pixel(0)
        , format(0)
        , generate_mipmaps(true)
        , type(TextureType::NONE)
    {
        init_stbi();
    }

    //Texture2D::Texture2D(const std::string& file_path, const std::string& type, bool generate_mipmaps)
    //    : id(0)
    //    , width(0)
    //    , height(0)
    //    , bytes_per_pixel(0)
    //    , format(0)
    //    , generate_mipmaps(true)
    //{
    //    init_stbi();
    //    load(file_path, type, generate_mipmaps);
    //}

    Texture2D::Texture2D(const std::string& file_path, TextureType type, bool generate_mipmaps)
        : id(0)
        , width(0)
        , height(0)
        , bytes_per_pixel(0)
        , format(0)
        , generate_mipmaps(true)
    {
        init_stbi();
        load(file_path, type, generate_mipmaps);
    }

    bool Texture2D::load(const std::string& file_path, TextureType t, bool generate_mipmaps)
    {
        i32 components_count;
        unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &components_count, 0);
        if (!data)
        {
            return false;
        }
        else
        {
            if (id != 0)
            {
                clear();
            }

            this->file_path = file_path;
            type = t;
            this->generate_mipmaps = generate_mipmaps;

            if (components_count == 1)
            {
                format = GL_RED;
            }
            else if (components_count == 3)
            {
                format = GL_RGB;
            }
            else if (components_count == 4)
            {
                format = GL_RGBA;
            }

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            if (generate_mipmaps)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(data);

            fmt::print("Loaded {} at {}\n", file_path, id);
        }

        return true;
    }

    Texture2D::~Texture2D()
    {
        fmt::print("Unloading texture {}\n", file_path);
        clear();
    }

    void Texture2D::bind(int texture_unit) const
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Texture2D::clear()
    {
        glDeleteTextures(1, &id);
    }
}