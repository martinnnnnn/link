#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <json.hpp>

#include "editor.hpp"
#include "link/data_root.hpp"
#include "link/types.hpp"
#include "link/file_system.hpp"

namespace link
{
    using nlohmann::json;

    struct EString
    {
        EString(const std::string& label, const std::string& value = "");

        std::string scope;
        std::string label;
        std::string value;
        std::string previous;
        //std::string hello2;

        void set(const std::string& new_value);

        void to_json(json& j, bool is_path)
        {
            if (is_path && !value.empty())
            {
                j["value"] = Path::get_path_relative(LINK_DATA_ROOT, value);
            }
            else
            {
                j["value"] = value;
            }
        }

        void from_json(const json& j, bool is_path)
        {
            std::string json_value = j["value"];
            if (is_path && !json_value.empty())
            {
                set(std::string(LINK_DATA_ROOT).append(json_value));
            }
            else
            {
                set(j.at("value"));
            }

        }

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw();
#else
        inline bool debug_draw() { return false; }
#endif


    };


}

// 09 77 41 03 81
