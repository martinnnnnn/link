#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <fmt/ostream.h>

#include "types.hpp"
#include "editor/editor.hpp"

namespace link
{
    namespace Path
    {
        inline std::string_view get_extention(const std::string& path)
        {
            size_t point = path.find_first_of(".");

            std::string_view path_view = path;
            path_view = path_view.substr(point + 1, path.length() - point);
            return path_view;
            //return path.substr(point + 1, (path.length() - 1) - point);
        }

        inline std::string_view get_path_relative(const::std::string& root, const std::string& path)
        {
            u32 lenght = root.length();
            std::string_view path_view = path;
            path_view = path_view.substr(0, lenght);
            return path_view;
            //return path.substr(lenght);
        }
    }


    struct FileSystem
    {
        struct Node
        {
            Node(const std::string& name, Node* parent, FileSystem* file_system);
            std::string get_absolute();
            std::string get_directory();
            u32 get_absolute_length();
            bool is_directory();

            std::string         name;
            Node*               parent;
            std::vector<Node*>  children;

            FileSystem* file_system = nullptr;
#ifdef LINK_EDITOR_ENABLED
            //bool is_selected = false;
            void debug_draw(u32 index);
#else
            inline void debug_draw(u32 index) {}
#endif
        };

        Node* root;

        void init(const std::string& root_path);
        void get_node(const std::string& path);

#ifdef LINK_EDITOR_ENABLED
        Node* selected = nullptr;
        void debug_draw();
#else
        inline void debug_draw() {}
#endif
    };
}