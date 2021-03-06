#include "file_system.hpp"

#include <imgui.h>
#include <assert.h>

#include "link/string_utils.hpp"

namespace link
{
    FileSystem::Node::Node(const std::string& name, Node* parent, FileSystem* file_system) : name(name), parent(parent), file_system(file_system)
    {
        std::string absolute = get_absolute();
        //fmt::print("New path node {} with absolute {}\n", name, absolute);

        if (is_directory())
        {
            for (const std::filesystem::directory_entry& p : std::filesystem::directory_iterator(absolute))
            {
                // clean path
                std::string path = p.path().string();
                string::replace(path, "\\", "/");

                // if it's a directory, we append a / to identify it as such and simplify path reconstruction
                if (p.is_directory())
                {
                    path += '/';
                }

                // get name
                std::string name = path.substr(absolute.length(), path.length() - absolute.length());

                Node* child = new Node(name, this, file_system);
                children.push_back(child);
            }
        }
    }

    std::string FileSystem::Node::get_absolute()
    {
        std::string path;
        path.reserve(get_absolute_length());
        if (parent)
        {
            path = parent->get_absolute();
        }
        path += name;
        return path;
    }

    std::string FileSystem::Node::get_directory()
    {
        std::string directory;

        if (is_directory())
        {
            directory = get_absolute();
        }
        else
        {
            assert(parent);

            directory = parent->get_absolute();
        }
        return directory;
    }

    u32 FileSystem::Node::get_absolute_length()
    {
        return name.length() + ((parent) ? parent->get_absolute_length() : 0);
    }

    bool FileSystem::Node::is_directory()
    {
        return name[name.length() - 1] == '/';
    }


    void FileSystem::init(const std::string& root_path)
    {
        root = new Node(root_path, nullptr, this);
    }

    void FileSystem::get_node(const std::string& path)
    {
        assert(false);
    }

#ifdef LINK_EDITOR_ENABLED
    void FileSystem::Node::debug_draw(u32 index)
    {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (file_system->selected == this)
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        if (is_directory())
        {
            if (ImGui::TreeNode(name.c_str()))
            {
                for (u32 i = 0; i < children.size(); ++i)
                {
                    children[i]->debug_draw(i);
                }
                ImGui::TreePop();
            }

            //bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, name.c_str());
            //for (u32 i = 0; i < children.size(); ++i)
            //{
            //    children[i]->debug_draw(i);
            //}
        }
        else
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, name.c_str());

            //if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            //{
            //    // Set payload to carry the index of our item (could be anything)
            //    std::string path = get_absolute();
            //    ImGui::SetDragDropPayload(Editor::DND_FILE_PATH, path.c_str(), path.length());

            //    // Display preview (could be anything, e.g. when dragging an image we could decide to display
            //    // the filename and a small preview of the image, etc.)
            //    ImGui::Text(name.c_str());
            //    ImGui::EndDragDropSource();
            //}

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                //uptr ptr = uptr(this);

                //ImGui::SetDragDropPayload(DND::c_str(DND::Target::FilePath), &ptr, sizeof(uptr));
                //fmt::print("dragging {}\n", ptr);
                //ImGui::Text(name.c_str());
                //ImGui::EndDragDropSource();

                uptr ptr = uptr(this);
                ImGui::SetDragDropPayload(DND::c_str(DND::Target::FilePath), &ptr, sizeof(uptr));
                ImGui::Text(name.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemClicked())
            {
                if (file_system->selected != this)
                {
                    file_system->selected = this;
                }
                else
                {
                    file_system->selected = nullptr;
                }
            }
        }
    }

    void FileSystem::debug_draw()
    {
        root->debug_draw(0);
    }
#endif
}

//// Our buttons are both drag sources and drag targets here!
//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
//{
//    // Set payload to carry the index of our item (could be anything)
//    ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));
//
//    // Display preview (could be anything, e.g. when dragging an image we could decide to display
//    // the filename and a small preview of the image, etc.)
//    if (mode == Mode_Copy) { ImGui::Text("Copy %s", names[n]); }
//    if (mode == Mode_Move) { ImGui::Text("Move %s", names[n]); }
//    if (mode == Mode_Swap) { ImGui::Text("Swap %s", names[n]); }
//    ImGui::EndDragDropSource();
//}
//if (ImGui::BeginDragDropTarget())
//{
//    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
//    {
//        IM_ASSERT(payload->DataSize == sizeof(int));
//        int payload_n = *(const int*)payload->Data;
//        if (mode == Mode_Copy)
//        {
//            names[n] = names[payload_n];
//        }
//        if (mode == Mode_Move)
//        {
//            names[n] = names[payload_n];
//            names[payload_n] = "";
//        }
//        if (mode == Mode_Swap)
//        {
//            const char* tmp = names[n];
//            names[n] = names[payload_n];
//            names[payload_n] = tmp;
//        }
//    }
//    ImGui::EndDragDropTarget();
//}